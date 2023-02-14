#include "relay.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  SCENARIO("", "[relay]") {
    GIVEN("normally open, no timeout") {
      auto uut = Relay{true, false};

      REQUIRE_FALSE(uut.closed());
      REQUIRE_FALSE(uut.pending());

      WHEN("toggling") {
        uut.toggle();

        uut(Relay_Timeout + 1);

        THEN("nothing happens") {
          REQUIRE_FALSE(uut.closed());
          REQUIRE(uut.pending());

          WHEN("forcing the switch") {
            uut.force();

            THEN("relay closes") {
              REQUIRE(uut.closed());
              REQUIRE_FALSE(uut.pending());
            }
          }
        }
      }
    }

    GIVEN("normally open, with timeout") {
      auto uut = Relay{true, true};

      REQUIRE_FALSE(uut.closed());
      REQUIRE_FALSE(uut.pending());

      WHEN("toggling") {
        uut.toggle();

        THEN("initially, nothing happens") {
          REQUIRE_FALSE(uut.closed());
          REQUIRE(uut.pending());

          uut(Relay_Timeout - 1);

          REQUIRE_FALSE(uut.closed());
          REQUIRE(uut.pending());

          WHEN("timeout expires") {
            uut(1);

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
