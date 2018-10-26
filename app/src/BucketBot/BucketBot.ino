#include <stdint.h>
#include <crc_table.h>
#include <N64.h>
#include <N64Controller.h>
#include <ICSC.h>
#include <Arduino.h>

/*-------------- Private Constants ---------- */
#define MAX_SPEED 3200

/* Pololu Protocol ID*/
#define PROTOCOL_ID 0xAA

/* Motor Controller Commands */
#define FORWARD 0x05
#define REVERSE 0x06
#define SAFE_START 0x03

/* Pololu protocol command buffer sizes */
#define CMD_SET_SAFE_LEN 0x03
#define CMD_SET_SPEED_LEN 0x05

/* Pololu Command Errors */
#define CMD_RESULT_OK 0x00
#define CMD_RESULT_ERROR 0xFF

/*-------------- Private Typedefs ---------- */
typedef enum
{
   MOTOR_ID_LEFT = 0x0A,
   MOTOR_ID_CENTER = 0x0B,
   MOTOR_ID_RIGHT = 0x0C
} MotorId_t;

typedef enum
{
   MOTOR_STATUS_OK,
   MOTOR_STATUS_BAD_CMD,
   MOTOR_STATUS_NULL_PTR,
   MOTOR_STATUS_BAD_PARAM
} MotorStatus_t;

typedef struct
{
   int32_t left;
   int32_t right;
} BiMotorPositions_t;

typedef struct
{
   int32_t x;
   int32_t y;
} Joystick_t;

typedef struct
{
   int32_t minCtrlOutput;
   int32_t maxCtrlOutput;
   int32_t minMotorCtrlInput;
   int32_t maxMotorCtrlInput;
} NormalizedMotorConfig_t;

/*-------------- Private Variables ---------- */
Stream *motorPtr = 0;

/*-------------- Private Prototypes ---------- */
void adjustVariance(Joystick_t *joystick);
static void updateBot(N64Controller_t controller);
static MotorStatus_t initMotorController(Stream *port);
static void driveSkidSteer(N64Controller_t controller);
static MotorStatus_t setExitSafeStart(MotorId_t motorId);
static void driveTorsoActuator(N64Controller_t controller);
static MotorStatus_t executeCommand(uint8_t *buf, size_t len);
BiMotorPositions_t transformedMotorPositionsV2(Joystick_t joystick);
float normalizeMotorSpeed(int32_t speed, NormalizedMotorConfig_t config);
static MotorStatus_t setMotorSpeedWithId(MotorId_t motorId, int32_t speed);
static void handlePacket(unsigned char src, char command, unsigned char len, char *data);

ICSC icsc(Serial, 'B');

void setup()
{
   Serial.begin(115200);
   initMotorController(Serial1);
   icsc.begin();
   icsc.registerCommand('C', &handlePacket);
}

void loop()
{
   icsc.process();
}

static void handlePacket(unsigned char src, char command, unsigned char len, char *data)
{
   N64Controller_t *controller = (N64Controller_t *)data;

   updateBot(*controller);
}

static void updateBot(N64Controller_t controller)
{
   /* Drive left/right motors on bot */
   driveSkidSteer(controller);
   /* Drive torso actuator */
   driveTorsoActuator(controller);
}

static void driveTorsoActuator(N64Controller_t controller)
{
   /* TODO */
}

static void driveSkidSteer(N64Controller_t controller)
{
   Joystick_t joystick = {0};
   NormalizedMotorConfig_t config = {0};

   /* Set joystick x/y thresholds */
   config.minCtrlOutput = -91;
   config.maxCtrlOutput = 91;

   /* Set Pololu motor controller thresholds */
   config.minMotorCtrlInput = -3200;
   config.maxMotorCtrlInput = 3200;

   joystick.x = (int32_t)normalizeMotorSpeed(controller.x, config);
   joystick.y = (int32_t)normalizeMotorSpeed(controller.y, config);

   adjustVariance(&joystick);
   BiMotorPositions_t positions = transformedMotorPositionsV2(joystick);

   setMotorSpeedWithId(MOTOR_ID_LEFT, controller.y);
   setMotorSpeedWithId(MOTOR_ID_RIGHT, controller.x);
}

float normalizeMotorSpeed(int32_t speed, NormalizedMotorConfig_t config)
{
   return ceil(config.minMotorCtrlInput +
               (float)(speed - config.minCtrlOutput) *
                   (float)(config.maxMotorCtrlInput - config.minMotorCtrlInput) /
                   (float)(config.maxCtrlOutput - config.minCtrlOutput));
}

BiMotorPositions_t transformedMotorPositionsV2(Joystick_t joystick)
{
   BiMotorPositions_t positions = {0};

   float Y = joystick.y;
   float X = -joystick.x;
   float V = (MAX_SPEED - (fabs(X))) * (Y / MAX_SPEED) + Y;
   float W = (MAX_SPEED - (fabs(Y))) * (X / MAX_SPEED) + X;
   float R = (V + W) / 2;
   float L = (V - W) / 2;

   positions.right = (int32_t)R;
   positions.left = (int32_t)L;

   return positions;
}

static void setStickDelta(int32_t *stick)
{
   float stickVariance = -100;
   if ((*stick <= 0 && *stick >= stickVariance) || (*stick >= 0 && *stick <= fabs(stickVariance)))
   {
      *stick = 0;
   }
}

void adjustVariance(Joystick_t *joystick)
{
   setStickDelta(&joystick->x);
   setStickDelta(&joystick->y);
}

static MotorStatus_t initMotorController(Stream *port)
{
   MotorStatus_t status = MOTOR_STATUS_OK;

   if (port != NULL)
   {
      /* Assign Serial[0-n] to our motor controller port */
      motorPtr = port;
   }
   else
   {
      /* Port is null, this is a deal breaker */
      status = MOTOR_STATUS_NULL_PTR;
   }

   return status;
}

static MotorStatus_t executeCommand(uint8_t *buf, size_t len)
{
   uint32_t sendCount = 0;
   MotorStatus_t status = MOTOR_STATUS_OK;

   /* Sanity check for null pointers */
   if ((buf != NULL) && (motorPtr != NULL))
   {
      /* Write buffer to serial port */
      sendCount = motorPtr->write(buf, len);

      /* If the sent command length does not equal CMD_SET_SPEED_LEN, return CMD_ERROR */
      status = (sendCount != len) ? MOTOR_STATUS_BAD_CMD : MOTOR_STATUS_OK;
   }
   else
   {
      /* buf or motorPtr is null */
      status = MOTOR_STATUS_NULL_PTR;
   }

   return status;
}

static MotorStatus_t setExitSafeStart(MotorId_t motorId)
{
   uint8_t bytes[CMD_SET_SAFE_LEN] = {PROTOCOL_ID, motorId, SAFE_START};

   /* Write to serial port */
   return executeCommand(bytes, sizeof(bytes));
}

static MotorStatus_t setMotorSpeedWithId(MotorId_t motorId, int32_t speed)
{
   uint32_t sendCount = 0;
   uint8_t bytes[CMD_SET_SPEED_LEN] = {PROTOCOL_ID, motorId, FORWARD, 0, 0};

   if (speed < 0)
   {
      /* Less than 0 is reverse */
      bytes[2] = REVERSE;
      speed = -speed;
   }
   else
   {
      /* Greater than 0 is forward */
      bytes[2] = FORWARD;
   }

   /* Direction assigned above, now assign the actual speed */
   bytes[3] = (uint8_t)(speed & 0x1F);
   bytes[4] = (uint8_t)(speed >> 5 & 0x7F);

   /* Write to serial port */
   return executeCommand(bytes, sizeof(bytes));
}
