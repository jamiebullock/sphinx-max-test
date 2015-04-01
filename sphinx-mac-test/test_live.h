//
//  test_live.h
//  sphinx-mac-test
//
//  Created by Jamie Bullock on 23/03/2015.
//  Copyright (c) 2015 Jamie Bullock. All rights reserved.
//

#ifndef sphinx_mac_test_test_live_h
#define sphinx_mac_test_test_live_h

#include <stdint.h>
#include <stdbool.h>

typedef struct aa_sphinx_data_ aa_sphinx_data;

aa_sphinx_data *new_aa_sphinx(void);
void free_aa_sphinx(aa_sphinx_data *aa_sphinx);
int init_decoder(aa_sphinx_data *aa_sphinx, const char *modeldir);
void deinit_decoder(aa_sphinx_data *aa_sphinx);
bool do_recognition(aa_sphinx_data *aa_sphinx, int16_t *adbuf, uint16_t bufsize, char *hyp, size_t hypsize);
const char *get_speech_string(aa_sphinx_data *aa_sphinx);


#endif
