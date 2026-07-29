#include "test_utility.h"

/** COLORS!
 * RED:    "\x1b[31m"
 * GREEN:  "\x1b[32m"
 * YELLOW: "\x1b[33m"
 * BLUE:   "\x1b[34m"
 *
 * RESET:  "\x1b[0m"
 */

int main() {
	printf("[INFO] Initializing: Robustness tests for OpenGL SC 2.0\n");
	printf("--------------------------------------------------\n");

	init();
	draw();
	cleanup();

	printf("--------------------------------------------------\n");
	printf("[SUCCESS] All tests complete.\n");

	return 0;
}
