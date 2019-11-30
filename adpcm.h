struct codec_state {
	int ps;	/* Predicted Sample */
	int i;	/* Index */
	int ss;	/* Step Size */
};

extern int8_t i_table[16];
extern int16_t ss_table[89];
extern struct codec_state state;

extern uint8_t adpcm_encode(int16_t in, struct codec_state *st);
extern int16_t adpcm_decode(uint8_t s, struct codec_state *st);
extern void state_reset(struct codec_state *st);
