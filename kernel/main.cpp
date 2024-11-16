#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "limine.h"
#include "printf.h"
#include "tty.hpp"

namespace {
  __attribute__((used, section(".requests"))) volatile LIMINE_BASE_REVISION(2);
}

namespace {
  __attribute__((used, section(".requests"))) volatile limine_framebuffer_request
  framebuffer_request = {

    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr,
  };
}

namespace {

  __attribute__((used, section(".requests_start_marker"))) volatile LIMINE_REQUESTS_START_MARKER;

  __attribute__((used, section(".requests_end_marker"))) volatile LIMINE_REQUESTS_END_MARKER;
}

extern "C" {
  void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = static_cast<uint8_t *>(dest);
    const uint8_t *psrc = static_cast<const uint8_t *>(src);

    for (size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
    }

    return dest;
  }

  void *memset(void *s, int c, size_t n) {
    uint8_t *p = static_cast<uint8_t *>(s);

    for (size_t i = 0; i < n; i++) {
      p[i] = static_cast<uint8_t>(c);
    }

    return s;
  }

  void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = static_cast<uint8_t *>(dest);
    const uint8_t *psrc = static_cast<const uint8_t *>(src);

    if (src > dest) {
      for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
      }
    } else if (src < dest) {
      for (size_t i = n; i > 0; i--) {
        pdest[i-1] = psrc[i-1];
      }
    }

    return dest;
  }

  int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = static_cast<const uint8_t *>(s1);
    const uint8_t *p2 = static_cast<const uint8_t *>(s2);

    for (size_t i = 0; i < n; i++) {
      if (p1[i] != p2[i]) {
        return p1[i] < p2[i] ? -1 : 1;
      }
    }

    return 0;
  }
}

namespace {
  void hcf() {
    asm ("cli");
    for (;;) {
      asm ("hlt");
    }
  }
}


// Extern declarations for global constructors array.
// Why do I need this?
extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C" void _start() {
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  // Call global constructors.
  // I believe this has something to do with GCC?
  for (size_t i = 0; &__init_array[i] != __init_array_end; i++) {
    __init_array[i]();
  }

  // Ensure we got a framebuffer.
  if (framebuffer_request.response == nullptr || framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  // Fetch the first framebuffer.
  limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  // Note: we assume the framebuffer model is RGB with 32-bit pixels.
  for (size_t i = 0; i < 1000; i++) {
	volatile uint32_t *fb_ptr =
      static_cast<volatile uint32_t *>(framebuffer->address);
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
  }

  printf("______  ___   _   _ _____ _____ \n");
  printf("|  _  \\/ _ \\ | \\ | |  _  /  ___|\n");
  printf("| | | / /_\\ \\|  \\| | | | \\ `--. \n");
  printf("| | | |  _  || . ` | | | |`--. \\\n");
  printf("| |/ /| | | || |\\  \\ \\_/ /\\__/ /\n");
  printf("|___/ \\_| |_/\_| \\_/\\___/\\____/ \n");

  printf("\nWelcome to the DANOS kernel\n");

  hcf();
}
