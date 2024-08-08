#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  struct dpu_set_t set, dpu;
  const char *dpu_binary;

  // Check if DPU_BINARY is provided as an argument
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <DPU_BINARY>\n", argv[0]);
    return 1;
  }

  dpu_binary = argv[1];

  char *dpu_alloc_profile = "backend=simulator";
  // Allocate DPUs with simulator backend
  DPU_ASSERT(dpu_alloc(4, dpu_alloc_profile, &set));

  DPU_ASSERT(dpu_load(set, dpu_binary, NULL));
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_log_read(dpu, stdout));
  }

  DPU_ASSERT(dpu_free(set));

  return 0;
}
