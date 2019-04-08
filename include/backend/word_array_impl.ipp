#ifndef WORD_ARRAY_IMPL_IPP
#define WORD_ARRAY_IMPL_IPP

#include <type_traits>
#include <cstdint>


template <unsigned int W, bool is_signed>
class WordArrayWrapper : public hint_base<W, is_signed, WordArrayWrapper>
{
public:
    using type = WordArrayWrapper<W, is_signed>;

    constexpr static int NW=(W+31)/32;

    using storage_type = std::array<uint32_t,NW>;

private:
    template<unsigned int N, bool val>
    friend class WordArrayWrapper;

    constexpr static int MSW_W    = W==0 ? 0 : ((W%32)==0) ? 32 : (W%32);
    constexpr static int MSW_MASK = MSW_W==0 ? 0 : MSW_W==32 ? 0xFFFFFFFFul : (0xFFFFFFFFul>>(32-MSW_W));

    storage_type m_storage;

    // Make sure the value is in range.
    static uint32_t clamp_msw(uint32_t x)
    {
        return x&MSW_MASK;
    }

    static void clamp(std::array<uint32_t,NW> &x)
    {
        x[NW-1]=x[NW-1]&MSW_MASK;
    }


public:
    template<unsigned int N, bool sign>
    using storage_helper = typename WordArrayWrapper<N,sign>::storage_type;

    template<unsigned int N, bool sign>
    using wrapper_helper = WordArrayWrapper<N, sign>;

    template<unsigned int N, bool sign>
    using wrapper_type = WordArrayWrapper<N, sign>;

    WordArrayWrapper(storage_type const & val)
        : m_storage{val}
    {
        std::cerr<<"W="<<W<<", NW="<<NW<<", val[NW-1]="<<val[NW-1]<<"\n";
        assert( val[NW-1] <= MSW_MASK );
    }
public:

    explicit WordArrayWrapper(uint64_t value)
    {
        if(W==0){
           assert(value==0);
        }else if(W<=32){
            assert(value <= MSW_MASK);
            m_storage[0]=value;
        }else{
            std::cerr<<"W="<<W<<", MSW_W="<<MSW_W<<", MSW_MASK="<<MSW_MASK<<", x="<<value<<"\n";
            assert( (value>>32) <= MSW_MASK);
            m_storage[0]=value;
            m_storage[1]=value>>32;
        }
    }


    // low can only be of type int or unsigned int using ac_int
    // TODO: Why does slicing a signed number produce an unsigned number?
    template<unsigned int high, unsigned int low>
    inline WordArrayWrapper<high - low + 1, false> do_slicing() const
    {
        using return_type = WordArrayWrapper<high - low + 1, false>;
        typename return_type::storage_type buffer{};
        const int MAJOR=low/32;
        const int MINOR=low%32;
        const int MINOR_REV = MINOR==0 ? 0 : 32-MINOR;
        if(MINOR==0){
            for(unsigned i=0; i<buffer.size(); i++){
                buffer[i]=m_storage[i+MAJOR];
            }
        }else{
            for(unsigned i=0; i<buffer.size(); i++){
                uint32_t val=m_storage[i+MAJOR]>>MINOR;
                if(i+MAJOR<NW){
                    val=val | (m_storage[i+MAJOR+1]<<MINOR_REV);
                }
                buffer[i]=val;
            }
        }
        return_type::clamp(buffer);
        return return_type{ buffer };
    }

    template<unsigned int idx>
    inline WordArrayWrapper<1, false> do_get() const
    {
        static_assert(idx<W, "Index out of range.");
        using return_type = WordArrayWrapper<1, false>;
        return return_type(
            W==0 ? 0 : (m_storage[idx/32]>>(idx%32)) & 1
        );
    }

    template<unsigned int idx>
    inline bool do_isset() const
    {
        static_assert(idx<W, "Index out of range.");
        return W==0 ? 0 : (m_storage[idx/32]>>(idx%32))&1;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    inline WordArrayWrapper<Wrhs + W, false> do_concatenate(
            WordArrayWrapper<Wrhs, isSignedRhs> const & rhs
        ) const
    {
        #error "This is currently broken..."
        std::cerr<<"concat<"<<W<<", "<<Wrhs<<")\n";
        using return_type = WordArrayWrapper<Wrhs + W, false>;
        typename return_type::storage_type res{};

        if(W==0){
            for(unsigned i=0; i<rhs.m_storage.size(); i++){
                res[i]=rhs.m_storage[i];
            }
        }else if(Wrhs==0){
            for(unsigned i=0; i<m_storage.size(); i++){
                res[i]=m_storage[i];
            }
        }else{

            for(unsigned i=0; i<Wrhs/32; i++){
               res[i]=rhs.m_storage[i];
            }
            if((Wrhs%32)==0){
                for(unsigned i=0; i<W/32; i++){
                    res[i+Wrhs]=m_storage[i];
                }
            }else{
                // Note that we already guaranteed that both lhs and rhs are non-zero width
                // This is super lazy and may be difficult for the optimiser to deal with, but easy to do...
                const int MAJOR=Wrhs/32;
                const int MINOR=Wrhs%32;
                for(unsigned i=0; i<W/32; i++){
                    uint32_t v=m_storage[i];
                    res[i+MAJOR] |= v<<MINOR;
                    res[i+MAJOR+1] |= v>>(32-MINOR);
                }
            }
        }
        return return_type{ res };
    }

    WordArrayWrapper<1, false> compare(WordArrayWrapper<W, is_signed> const & rhs)const
    {
        for(unsigned i=0; i<NW; i++){
            if(m_storage[i] != rhs.m_storage[i]){
                return WordArrayWrapper<1, false>{ {0} };
            }
        }
        return WordArrayWrapper<1, false>{ {1} };
    }


    // TODO: To me this seems mis-named. It is reinterpreting bits, but this makes it sound
    // like it is creating logic.
    inline WordArrayWrapper<W, !is_signed> invert_sign() const
    {
        return WordArrayWrapper<W, !is_signed>{m_storage};
    }


    // TODO: Not entirely sure what this is doing by the name. Is it generating
    // W replicationns of the input bit?
    static inline WordArrayWrapper<W, false> do_generateSequence(
            WordArrayWrapper<1, false> const & val
            )
    {
        storage_type res{};
        if(val.m_storage[0]){
            res.fill(0xFFFFFFFFul);
            clamp(res);
        }
        return WordArrayWrapper<W, false>( res );
    }

    wrapper_helper<W+1, is_signed> perform_addc(
            wrapper_helper<W, is_signed> const & op2,
            wrapper_helper<1, false> const & cin
        ) const
    {
        using return_type = wrapper_helper<W+1, is_signed>;
        typename return_type::storage_type buffer{};
        uint64_t carry=cin.m_storage[0];
        for(unsigned i=0; i<W; i++){
            carry += m_storage[i] + op2.m_storage[i];
            buffer[i]=carry;
            carry=carry>>32;
        }
        if((W%32)==0){
            buffer[W]=carry;
        }
        return_type::clamp(buffer);
        return return_type(buffer);
    }

    static inline wrapper_helper<W, is_signed> do_mux(
            wrapper_helper<1, false> const & control,
            wrapper_helper<W, is_signed> const & opt1,
            wrapper_helper<W, is_signed> const & opt0
        )
    {
        return control.m_storage[0] ? opt1 : opt0;
    }
};


#endif
