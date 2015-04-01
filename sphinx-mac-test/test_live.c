//system headerfiles

#include "test_live.h"

#include <sphinxbase/ad.h>
#include <sphinxbase/err.h>
#include <pocketsphinx.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>

#define MODELDIR "/tmp/model"

struct aa_sphinx_data_
{
    ad_rec_t *ad_rec;
    ps_decoder_t *ps;
    uint8 utt_started;
    uint8 in_speech;
    int32 k;
    char const *hyp;
};

bool do_recognition(aa_sphinx_data *aa_sphinx, int16_t *adbuf, uint16_t bufsize, char *hyp, size_t hypsize)
{
    bool success = false;
    
    if ((aa_sphinx->k = ad_read(aa_sphinx->ad_rec, adbuf, bufsize)) < 0)
        E_FATAL("Failed to read audio\n");
    ps_process_raw(aa_sphinx->ps, adbuf, aa_sphinx->k, FALSE, FALSE);
    aa_sphinx->in_speech = ps_get_in_speech(aa_sphinx->ps);
    if (aa_sphinx->in_speech && !aa_sphinx->utt_started) {
        aa_sphinx->utt_started = TRUE;
//        printf("Listening...\n");
    }
    if (!aa_sphinx->in_speech && aa_sphinx->utt_started) {
        /* speech -> silence transition, time to start new utterance  */
        ps_end_utt(aa_sphinx->ps);
        char const *hyp_ = ps_get_hyp(aa_sphinx->ps, NULL );
        if (hyp != NULL)
        {
            snprintf(hyp, hypsize, "%s\n", hyp_);
//            printf("%s\n", hyp_);
            success = true;
        }
        if (ps_start_utt(aa_sphinx->ps) < 0)
            E_FATAL("Failed to start utterance\n");
        aa_sphinx->utt_started = FALSE;
//        printf("READY....\n");
    }
    return success;
}

const char *get_speech_string(aa_sphinx_data *sphinx_data)
{
    return sphinx_data->hyp;
}

static jmp_buf jbuf;
static void sighandler(int signo)
{
    longjmp(jbuf, 1);
}

aa_sphinx_data *new_aa_sphinx(void)
{
    return malloc(sizeof(aa_sphinx_data));
}

void free_aa_sphinx(aa_sphinx_data *aa_sphinx)
{
    free(aa_sphinx);
}


void deinit_decoder(aa_sphinx_data *aa_sphinx)
{
    ad_close(aa_sphinx->ad_rec);
    ps_free(aa_sphinx->ps);
}

int init_decoder(aa_sphinx_data *aa_sphinx, const char *modeldir)
{
    
    char hmmpath[PATH_MAX];
    char lmpath[PATH_MAX];
    char dictpath[PATH_MAX];
    
    snprintf(hmmpath, PATH_MAX, "%s/%s", modeldir, "en-us/en-us");
    snprintf(lmpath, PATH_MAX, "%s/%s", modeldir, "en-us/en-us.lm.dmp");
    snprintf(dictpath, PATH_MAX, "%s/%s", modeldir, "en-us/cmudict-en-us.dict");
    
    printf("hmmpath: %s\n", hmmpath);
    printf("lmpath: %s\n", lmpath);
    printf("dictpath: %s\n", dictpath);
    
    cmd_ln_t *config = cmd_ln_init(NULL, ps_args(), TRUE,
                                  "-logfn", "/dev/null",
                                   "-hmm", hmmpath,
                                   "-lm", lmpath,
                                   "-dict", dictpath,
                                   NULL);
    
    aa_sphinx->in_speech = FALSE;
    aa_sphinx->utt_started = FALSE;
    
    aa_sphinx->hyp = NULL;
    
    if (config == NULL)
        return -1;
    
    ps_default_search_args(config);
    
    aa_sphinx->ps = ps_init(config);
    
    if (aa_sphinx->ps == NULL) {
        cmd_ln_free_r(config);
        return -1;
    }
    
    signal(SIGINT, &sighandler);
    
    if ((aa_sphinx->ad_rec = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
                          (int) cmd_ln_float32_r(config,
                                                 "-samprate"))) == NULL)
        E_FATAL("Failed to open audio device\n");
    if (ad_start_rec(aa_sphinx->ad_rec) < 0)
        E_FATAL("Failed to start recording\n");
    
    if (ps_start_utt(aa_sphinx->ps) < 0)
        E_FATAL("Failed to start utterance\n");
    
    printf("READY....\n");
    return 0;
}





