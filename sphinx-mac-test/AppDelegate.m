//
//  AppDelegate.m
//  sphinx-mac-test
//
//  Created by Jamie Bullock on 23/03/2015.
//  Copyright (c) 2015 Jamie Bullock. All rights reserved.
//

#import "AppDelegate.h"

#include "test_live.h"

@interface AppDelegate ()
{
    
}
@property (unsafe_unretained) IBOutlet NSTextView *textView;

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    NSString *path = [[NSBundle mainBundle] bundlePath];
    path = [path stringByAppendingPathComponent:@"/Contents/Resources/model"];
    NSLog(@"%@", path);
    
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        
        struct timespec ts;
        
        ts.tv_sec = 0;
        ts.tv_nsec = 100 * 1000000;
        
        aa_sphinx_data *aa_sphinx = new_aa_sphinx();
        int rv = init_decoder(aa_sphinx, [path UTF8String]);
        uint16_t bufsize = 2048;
        int16_t adbuf[2048];
        char hyp[8192];
        size_t hypsize = 8192;

        if (!rv) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self->_textView insertText:@"READY..."];
            });
        }
        else
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self->_textView insertText:@"INIT FAILED"];
            });
            return;
        }
        
        while (1)
        {
            bool success = do_recognition(aa_sphinx, adbuf, bufsize, hyp, hypsize);
            if (success)
            {
                NSString *recognized = [NSString stringWithUTF8String:hyp];
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self->_textView insertText:recognized];
                });
            }
            nanosleep(&ts, NULL);
        }
//        deinit_decoder(aa_sphinx);
//        free_sphinx_data(aa_sphinx);
    });
    
  
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
