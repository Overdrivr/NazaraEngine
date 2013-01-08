#ifndef BATCH_HPP
#define BATCH_HPP


class NzBatch
{
    public:
        NzBatch(unsigned int start, unsigned int count);
        ~NzBatch();

        bool Add(unsigned int index);
        int Remove(unsigned int index, NzBatch& newBatch);

        bool Contains(unsigned int index) const;
        unsigned int Count() const;

        bool IsEmpty() const;

        bool IsAfter(unsigned int index) const;
        bool IsAtLast(unsigned int index) const;
        bool IsBefore(unsigned int index) const;
        bool IsRightAfter(unsigned int index) const;
        bool IsRightBefore(unsigned int index) const;

        bool MergeWith(const NzBatch& batch);

        unsigned int Start() const;

    protected:
    private:
        unsigned int m_index;
        unsigned int m_count;
};

#endif // BATCH_HPP
