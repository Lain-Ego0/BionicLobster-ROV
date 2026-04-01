#ifndef __ACTION_H__
#define __ACTION_H__

#include "control_link.h"

void Start(void);
void MoveForward(void);
void MoveBackward(void);
void MoveLeft(void);
void MoveRight(void);
void TurnLeft(void);
void TurnRight(void);
void Ascend(void);
void Descend(void);
void ApplyBoardControlCommand(const BoardControlCommand *command);

#endif
