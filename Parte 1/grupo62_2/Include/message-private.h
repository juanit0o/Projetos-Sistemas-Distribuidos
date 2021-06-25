//Grupo 62: Pedro Afonso Neves fc46430
//Ricardo Calçado fc46382
//Tiago Ferreira fc47895

#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "entry.h"
#include "message.h"

#define _SHORT 2
#define _INT 4

/* Define os possíveis opcodes da mensagem */
#define OC_RT_ERROR	99

#define CT_NO_DATA 5

/* Define códigos para os possíveis conteúdos da mensagem */
#define ERR_CODE -1
#define ERR_CODE_OUT_OF_RANGE -2
#define ERR_CODE_TABLE_FULL -40
#define ERR_CODE_NO_UPDATE -20

#endif
