enum DataLength {
    DATA2BYTES = 2,
    DATA4BYTES = 4,
    DATA6BYTES = 6
};
void InverterReset(const char sNum[3]);
void ParamaterWrite(const char sNum[3],const char par[3],const char val[5]);
void ParameterRead(const char sNum[3],const char par[3]);
