struct codec_state {
	int ps;	/* Predicted Sample */
	int i;	/* Index */
	int ss;	/* Step Size */
};

typedef struct codec_state STATE;

extern int8_t i_table[16];
extern int16_t ss_table[89];

extern uint8_t adpcm_encode(int16_t in, STATE *st);
extern int16_t adpcm_decode(uint8_t s, STATE *st);
extern STATE *state_init(int predsmp, int index, int stepsize);
extern void state_free(STATE *st);
