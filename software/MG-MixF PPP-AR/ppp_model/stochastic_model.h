#ifndef STOCHASTIC_MODEL_H
#define STOCHASTIC_MODEL_H
#include "pos_algorith.h"
#include"AGNSS_APPData.h"
class stochastic_model
{
public:
    stochastic_model();
    void sine_Wight(SatlitData &tempSatlitData);
    void cosine_Wight(SatlitData &tempSatlitData);
    void sine1_Wight(SatlitData &tempSatlitData);
    void panda_Wight(SatlitData &tempSatlitData);
};

#endif // STOCHASTIC_MODEL_H
