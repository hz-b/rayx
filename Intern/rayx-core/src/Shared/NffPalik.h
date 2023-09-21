#ifndef NFF_PALIK_H
#define NFF_PALIK_H

/** This struct represents one line of a .NKP file.  */
struct PalikEntry {
    float m_energy;
    float m_n;
    float m_k;
};

/** This struct represents one line of a .nff file.  */
struct NffEntry {
    float m_energy;
    float m_f1;
    float m_f2;
};

#endif
