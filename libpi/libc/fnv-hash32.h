#ifndef __FNV_HASH_H__
#define __FNV_HASH_H__

static inline uint32_t 
fnv_hash32(uint32_t *v, uint32_t n, uint32_t hash) {
    uint32_t fnv_prime = 16777619;
    for(unsigned i = 0; i < n; i++) {
        hash = hash ^ v[i];
        hash = hash * fnv_prime;
    }
    return hash;
}

#if 0
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#endif

// assumes 17 registers
static inline uint32_t 
fnv_hash32_regs(uint32_t *v, uint32_t n, uint32_t hash) {
    if(unlikely(n!=17))
        panic("expected 17, have %d\n", n);

    uint32_t fnv_prime = 16777619;

    uint32_t v0     = v[0];
    uint32_t v1     = v[1];
    uint32_t v2     = v[2];
    uint32_t v3     = v[3];
    uint32_t v4     = v[4];
    uint32_t v5     = v[5];
    uint32_t v6     = v[6];
    uint32_t v7     = v[7];
    uint32_t v8     = v[8];
    uint32_t v9     = v[9];
    uint32_t v10    = v[10];
    uint32_t v11    = v[11];
    uint32_t v12    = v[12];
    uint32_t v13    = v[13];
    uint32_t v14    = v[14];
    uint32_t v15    = v[15];
    uint32_t v16    = v[16];

    hash = hash ^ v0; hash = hash * fnv_prime;
    hash = hash ^ v1; hash = hash * fnv_prime;
    hash = hash ^ v2; hash = hash * fnv_prime;
    hash = hash ^ v3; hash = hash * fnv_prime;
    hash = hash ^ v4; hash = hash * fnv_prime;
    hash = hash ^ v5; hash = hash * fnv_prime;
    hash = hash ^ v6; hash = hash * fnv_prime;
    hash = hash ^ v7; hash = hash * fnv_prime;
    hash = hash ^ v8; hash = hash * fnv_prime;
    hash = hash ^ v9; hash = hash * fnv_prime;
    hash = hash ^ v10; hash = hash * fnv_prime;
    hash = hash ^ v11; hash = hash * fnv_prime;
    hash = hash ^ v12; hash = hash * fnv_prime;
    hash = hash ^ v13; hash = hash * fnv_prime;
    hash = hash ^ v14; hash = hash * fnv_prime;
    hash = hash ^ v15; hash = hash * fnv_prime;
    hash = hash ^ v16; hash = hash * fnv_prime;

    return hash;
}

#endif
