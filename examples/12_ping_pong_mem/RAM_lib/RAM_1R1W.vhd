library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

package RAM_1R1W_pkg is
  component RAM_1R1W
    generic (
      words      : natural := 16;
      width      : natural := 16;
      addr_width : natural := 4);
    port (
      CK   : in  std_logic;
      RCSN : in  std_logic;
      WCSN : in  std_logic;
      RA   : in  std_logic_vector(addr_width - 1 downto 0);
      WA   : in  std_logic_vector(addr_width - 1 downto 0);
      D    : in  std_logic_vector(width - 1 downto 0);
      Q    : out std_logic_vector(width - 1 downto 0));
  end component;
end RAM_1R1W_pkg;



library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library mgc_hls;
use mgc_hls.RAM_1R1W_pkg.all;

entity RAM_1R1W is
  generic (
    words      : natural := 16;
    width      : natural := 16;
    addr_width : natural := 4);
  port (
    CK   : in  std_logic;
    RCSN : in  std_logic;
    WCSN : in  std_logic;
    RA   : in  std_logic_vector(addr_width - 1 downto 0);
    WA   : in  std_logic_vector(addr_width - 1 downto 0);
    D    : in  std_logic_vector(width - 1 downto 0);
    Q    : out std_logic_vector(width - 1 downto 0));
end RAM_1R1W;

architecture sim of RAM_1R1W is
  type mem_type is array ((words)-1 downto 0) of std_logic_vector(width - 1 downto 0);
  signal mem : mem_type;
  begin
-- synopsys translate_off
    I0 : process (CK)
      begin
        if ( CK'event and CK = '1' ) then
          if ( RCSN = '0' ) then
            if ( RA = WA and WCSN = '0' ) then  -- read-write contention
              Q <= (others => 'X');
            else
              Q <= mem(conv_integer(RA));
            end if;
          end if;
          if ( WCSN = '0' ) then
            mem(conv_integer(WA)) <= D;
          end if;
        end if;
    end process;
-- synopsys translate_on
end sim;
