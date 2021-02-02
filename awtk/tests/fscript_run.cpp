#include "tkc/mem.h"
#include "tkc/utils.h"
#include "tkc/platform.h"
#include "tkc/time_now.h"
#include "tkc/fscript.h"
#include "tkc/object_default.h"

int main(int argc, char* argv[]) {
  platform_prepare();

  tk_mem_dump();
  if (argc < 2) {
    printf("Usage: %s script\n", argv[0]);
    return 0;
  } else {
    value_t v;
    char buff[64];
    uint64_t start = time_now_us();
    const char* code = argv[1];
    object_t* obj = object_default_create();
    tk_mem_dump();
    if(argc == 3) {
      /*stress test*/
      uint32_t i = 0;
      uint32_t times = tk_atoi(argv[2]);
      fscript_t* fscript = fscript_create(obj, code);
      for(i = 0; i < times; i++) {
        fscript_exec(fscript, &v);
        value_reset(&v);
      }
      fscript_destroy(fscript);
    } else {
      fscript_eval(obj, code, &v);
      log_debug("result:%s\n", value_str_ex(&v, buff, sizeof(buff)-1));
      value_reset(&v);
    }
    OBJECT_UNREF(obj);
    log_debug("cost: %d us\n", (int)(time_now_us() - start));
  }
  tk_mem_dump();

  return 0;
}
