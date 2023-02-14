#ifndef BELMOOR_GPIO_HPP_
#define BELMOOR_GPIO_HPP_ 1

namespace belmoor {

  template <char Port_, unsigned PinMask_> struct GPIO_proxy {
    void set();
    void clear();

  };

} //

#endif // BELMOOR_GPIO_HPP_
