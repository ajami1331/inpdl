#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

class Processor
{
public:
    virtual ~Processor() {}
    virtual void Process() = 0;   
};

#endif // PROCESSOR_HPP