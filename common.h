#ifndef COMMON_H
#define COMMON_H

typedef enum {
	NO_BLOCK,
	DIRT,
	TOPSOIL
}t_Block_Type;

typedef struct
{
	t_Block_Type type;
}t_levelBlock;

#endif;