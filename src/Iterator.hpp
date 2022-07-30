#pragma once

template <typename V>
class Iterator
{
    Iterator();
    public:
    virtual bool has_next() = 0;
    virtual V next() = 0;
};

class IntIterator
{
    int position, end, increment;
public:
    IntIterator(int begin, int end): position(begin), end(end), increment(1) {}

    bool has_next()
    {
        return position < end;
    }

    int next()
    {
        int n = position;
        position += increment;
        return n;
    }
};
