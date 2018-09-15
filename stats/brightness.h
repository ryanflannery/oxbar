/* TODO Build a working brightness widget
 * OK - querying the X display's brightness is not terribly easy.
 * Checkout xbacklight's source for a good reference.
 * Lots of atoms... replys... all sorts of X goo.
 *
 * I'm tempted to just popen("xbacklight -get") and read that.
 * But fork/exec every second seems heavy.
 * Also my keys to control brightness don't work on my laptop, so I odn't
 * really care about this... :D
 */

typedef struct brightness {
   int   brightness;
} brightness_info_t;
extern brightness_info_t brightness;

