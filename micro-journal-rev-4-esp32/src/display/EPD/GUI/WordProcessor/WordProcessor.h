#ifndef WORDPROCESSOR_h
#define WORDPROCESSOR_h

//
void WP_setup();

// 
void WP_render();

// 
void WP_keyboard(char key);

//
void WP_render_clear();
void WP_render_status();
void WP_render_text();
void WP_render_cursor();    
void WP_check_saved();
void WP_check_sleep();

#endif