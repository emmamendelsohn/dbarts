#ifndef EXTERNAL_RANDOM_H
#define EXTERNAL_RANDOM_H

// create an RNG and then seed it, or create it using a state constructed from somewhere else

#include "stddef.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ext_rng ext_rng;

typedef enum {
    EXT_RNG_ALGORITHM_WICHMANN_HILL = 0,
    EXT_RNG_ALGORITHM_MARSAGLIA_MULTICARRY,
    EXT_RNG_ALGORITHM_SUPER_DUPER,
    EXT_RNG_ALGORITHM_MERSENNE_TWISTER,
    EXT_RNG_ALGORITHM_KNUTH_TAOCP,
    EXT_RNG_ALGORITHM_USER_UNIF,
    EXT_RNG_ALGORITHM_KNUTH_TAOCP2,
    EXT_RNG_ALGORITHM_LECUYER_CMRG
} ext_rng_algorithm_t;

typedef enum {
    EXT_RNG_STANDARD_NORMAL_BUGGY_KINDERMAN_RAMAGE = 0,
    EXT_RNG_STANDARD_NORMAL_AHRENS_DIETER,
    EXT_RNG_STANDARD_NORMAL_BOX_MULLER,
//    EXT_RNG_STANDARD_NORMAL_USER_NORM,
    EXT_RNG_STANDARD_NORMAL_INVERSION,
    EXT_RNG_STANDARD_NORMAL_KINDERMAN_RAMAGE
} ext_rng_standardNormal_t;


// state can be null; size is determined by algorithm; see below for a few state structs
ext_rng* ext_rng_create(ext_rng_algorithm_t algorithm, const void* v_state);
void ext_rng_destroy(ext_rng* generator);
int ext_rng_setStandardNormalAlgorithm(ext_rng* generator, ext_rng_standardNormal_t standardNormalAlgorithm);
int ext_rng_setSeed(ext_rng* generator, uint_least32_t seed);
int ext_rng_setSeedFromClock(ext_rng* generator);

double ext_rng_simulateContinuousUniform(ext_rng* generator); // randomBase.c
double ext_rng_simulateStandardNormal(ext_rng* generator);    // randomNorm.c

// standard normal truncated below at lowerBound, using Robert (1995)
double ext_rng_simulateLowerTruncatedStandardNormal(ext_rng* generator, double lowerBound); 
// use the previous to generate truncated normals with sd 1 and nonzero mean
double ext_rng_simulateLowerTruncatedNormalScale1(ext_rng* generator, double mean, double bound);
double ext_rng_simulateUpperTruncatedNormalScale1(ext_rng* generator, double mean, double bound);

// subsequent in random.c
double ext_rng_simulateExponential(ext_rng* generator, double scale);
double ext_rng_simulateGamma(ext_rng* generator, double shape, double scale);

#define ext_rng_simulateChiSquared(_GENERATOR_, _DF_) ext_rng_simulateGamma(_GENERATOR_, (_DF_) / 2.0, 2.0)
#define ext_rng_simulateBernoulli(_GENERATOR_, _P_) (ext_rng_simulateContinuousUniform(_GENERATOR_) < (_P_) ? 1u : 0u)
#define ext_rng_simulateNormal(_GENERATOR_, _MU_, _SIGMA_) (ext_rng_simulateStandardNormal(_GENERATOR_) * (_SIGMA_) + (_MU_))


#define EXT_DISCRETE_DRAW_FAILURE ((ext_size_t) -1)
ext_size_t ext_rng_drawFromDiscreteDistribution(ext_rng* generator, const double* probabilities, ext_size_t length);
  
// random in [min, min + 1, ..., max - 1, max)
int64_t ext_rng_simulateIntegerUniformInRange(ext_rng* generator, int64_t min_inclusive, int64_t max_exclusive);
uint64_t ext_rng_simulateUnsignedIntegerUniformInRange(ext_rng* generator, uint64_t min_inclusive, uint64_t max_exclusive);



#define EXT_RNG_KNUTH_NUM_RANDOM 100
#define EXT_RNG_KNUTH_QUALITY 1009

typedef struct {
  uint_least32_t state1[EXT_RNG_KNUTH_NUM_RANDOM];
  int_least32_t info;
  uint_least32_t state2[EXT_RNG_KNUTH_QUALITY];
} ext_rng_knuth_state;

#define EXT_RNG_MERSENNE_TWISTER_NUM_RANDOM 624

typedef struct {
  int_least32_t info;
  uint_least32_t state[EXT_RNG_MERSENNE_TWISTER_NUM_RANDOM];
} ext_rng_mersenne_twister_state;

typedef struct {
  double (*simulateContinuousUniform)(void* state);
  void* state;
} ext_rng_user_uniform_state;

#ifdef __cplusplus
}
#endif

#endif // EXTERNAL_RANDOM_H
