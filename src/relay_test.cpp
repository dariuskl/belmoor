#include "relay.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  SCENARIO("", "[relay]") {
    GIVEN("normally open") {
      auto uut = Relay{true};

      REQUIRE_FALSE(uut.closed());
      REQUIRE_FALSE(uut.pending());

      WHEN("toggling") {
        uut.toggle();

        THEN("nothing happens") {
          REQUIRE_FALSE(uut.closed());
          REQUIRE(uut.pending());

          WHEN("forcing the switch") {
            uut.update();

            THEN("relay closes") {
              REQUIRE(uut.closed());
              REQUIRE_FALSE(uut.pending());
            }
          }
        }
      }
    }
  }

} // namespace belmoor
