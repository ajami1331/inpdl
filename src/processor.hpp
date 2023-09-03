#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

class Processor
{
public:
    ~Processor() {}
    virtual void Process() = 0;   
};

#endif // PROCESSOR_HPP