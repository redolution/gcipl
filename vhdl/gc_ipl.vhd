library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity gc_ipl is
port
(
    -- GC side
    exi_do_phy : out STD_LOGIC_VECTOR(3 downto 0);
    exi_di : in STD_LOGIC;
    exi_cs : in STD_LOGIC;
    exi_clk : in STD_LOGIC;

    -- Flash
    f_miso : in STD_LOGIC;
    f_mosi_phy : out STD_LOGIC;
    f_cs_phy : out STD_LOGIC;
    f_sck : out STD_LOGIC;

    -- ATmega
    at_miso : out STD_LOGIC;
    at_mosi : in STD_LOGIC;
    at_cs : in STD_LOGIC;
    at_clk : in STD_LOGIC
);
end gc_ipl;

architecture Behavioral of gc_ipl is

    signal passthrough : STD_LOGIC;
    signal cancel : STD_LOGIC;

    -- This is a circular register used for command translation
    signal exi_buffer : STD_LOGIC_VECTOR(5 downto 0);

    -- Bit counter
    signal exi_count : integer range 0 to 31;

    -- Flash read command
    constant f_read : STD_LOGIC_VECTOR(7 downto 0) := x"03";

    -- Internal buffers
    --signal exi_do : STD_LOGIC;
    signal f_mosi : STD_LOGIC;
    signal f_cs : STD_LOGIC;

begin

    process (cancel, passthrough,
            exi_cs, exi_di, exi_clk,
            f_miso, f_mosi, f_cs,
            at_cs, at_mosi, at_clk)
    begin
        if (at_cs = '0')
        then
            -- Let the ATmega control flash
            at_miso <= f_miso;
            exi_do_phy <= (others => 'Z');
            f_sck <= at_clk;
            f_mosi_phy <= at_mosi;
            f_cs_phy <= '0';
        elsif ((exi_cs = '1') or (cancel = '1'))
        then
            -- Default idle state
            at_miso <= 'Z';
            exi_do_phy <= (others => 'Z');
            f_sck <= exi_clk;
            f_mosi_phy <= 'Z';
            f_cs_phy <= '1';
        elsif (passthrough = '1')
        then
            -- Direct passthrough EXI <=> flash
            at_miso <= 'Z';
            exi_do_phy <= (others => f_miso);
            f_sck <= exi_clk;
            f_mosi_phy <= exi_di;
            f_cs_phy <= '0';
        else
            -- We control flash
            at_miso <= 'Z';
            exi_do_phy <= (others => 'Z'); -- No data output to EXI while we control flash
            f_sck <= exi_clk;
            f_mosi_phy <= f_mosi;
            f_cs_phy <= f_cs;
        end if;
    end process;

    process (exi_clk, exi_cs)
    begin
        if (exi_cs = '1')
        then
            cancel <= '0';
            exi_count <= 0;

        -- Do our shit on clock rising edge (host does its stuff on falling edge)
        elsif (rising_edge(exi_clk))
        then
            -- Read command translator
            if ((cancel = '0') and (exi_cs = '0') and (passthrough = '0'))
            then
                case exi_count is
                    when 0 =>
                        f_mosi <= f_read(7);
                        -- If the GameCube is reading, enable flash
                        f_cs <= exi_di;

                    when 1 =>
                        f_mosi <= f_read(6);

                    when 2 =>
                        f_mosi <= f_read(5);
                        exi_buffer(0) <= exi_di;
                        -- If bit 2 is set, we're out of bounds so cancel the transfer now
                        if (exi_di = '1')
                        then
                            cancel <= '1';
                        end if;

                    when 3 =>
                        f_mosi <= f_read(4);
                        exi_buffer(1) <= exi_di;

                    when 4 =>
                        f_mosi <= f_read(3);
                        exi_buffer(2) <= exi_di;

                    when 5 =>
                        f_mosi <= f_read(2);
                        exi_buffer(3) <= exi_di;
                        --exi_buffer(3) <= '1';

                    when 6 =>
                        f_mosi <= f_read(1);
                        exi_buffer(4) <= exi_di;
                        --exi_buffer(4) <= '1';

                    when 7 =>
                        f_mosi <= f_read(0);
                        exi_buffer(5) <= exi_di;

                    -- Here goes the first bit of the address
                    when 8 =>
                        f_mosi <= exi_buffer(0);
                        exi_buffer(0) <= exi_di;

                    when 9 =>
                        f_mosi <= exi_buffer(1);
                        exi_buffer(1) <= exi_di;

                    when 10 =>
                        f_mosi <= exi_buffer(2);
                        exi_buffer(2) <= exi_di;

                    when 11 =>
                        f_mosi <= exi_buffer(3);
                        exi_buffer(3) <= exi_di;

                    when 12 =>
                        f_mosi <= exi_buffer(4);
                        exi_buffer(4) <= exi_di;

                    when 13 =>
                        f_mosi <= exi_buffer(5);
                        exi_buffer(5) <= exi_di;

                    when 14 =>
                        f_mosi <= exi_buffer(0);
                        exi_buffer(0) <= exi_di;

                    when 15 =>
                        f_mosi <= exi_buffer(1);
                        exi_buffer(1) <= exi_di;

                    when 16 =>
                        f_mosi <= exi_buffer(2);
                        exi_buffer(2) <= exi_di;

                    when 17 =>
                        f_mosi <= exi_buffer(3);
                        exi_buffer(3) <= exi_di;

                    when 18 =>
                        f_mosi <= exi_buffer(4);
                        exi_buffer(4) <= exi_di;

                    when 19 =>
                        f_mosi <= exi_buffer(5);
                        exi_buffer(5) <= exi_di;

                    when 20 =>
                        f_mosi <= exi_buffer(0);
                        exi_buffer(0) <= exi_di;

                    when 21 =>
                        f_mosi <= exi_buffer(1);
                        exi_buffer(1) <= exi_di;

                    when 22 =>
                        f_mosi <= exi_buffer(2);
                        exi_buffer(2) <= exi_di;

                    when 23 =>
                        f_mosi <= exi_buffer(3);
                        exi_buffer(3) <= exi_di;

                    when 24 =>
                        f_mosi <= exi_buffer(4);
                        exi_buffer(4) <= exi_di;

                    when 25 =>
                        f_mosi <= exi_buffer(5);
                        exi_buffer(5) <= exi_di;

                    -- No need to buffer the address anymore, we've received all of it now
                    when 26 =>
                        f_mosi <= exi_buffer(0);
                        --exi_buffer(0) <= exi_di;

                    when 27 =>
                        f_mosi <= exi_buffer(1);
                        --exi_buffer(1) <= exi_di;

                    when 28 =>
                        f_mosi <= exi_buffer(2);
                        --exi_buffer(2) <= exi_di;

                    when 29 =>
                        f_mosi <= exi_buffer(3);
                        --exi_buffer(3) <= exi_di;

                    when 30 =>
                        f_mosi <= exi_buffer(4);
                        --exi_buffer(4) <= exi_di;

                    when 31 =>
                        f_mosi <= exi_buffer(5);
                        --exi_buffer(5) <= exi_di;
                end case;

                exi_count <= exi_count + 1;
            end if;
        end if;
    end process;

    process (exi_clk, exi_cs)
    begin
        if (exi_cs = '1')
        then
            passthrough <= '0';
        elsif (falling_edge(exi_clk))
        then
            if (exi_count = 31)
            then
                -- Enable passthrough mode now
                -- If the GameCube was trying to write, flash is deselected so it
                -- will be selected on the next clock cycle allowing arbitrary
                -- commands
                passthrough <= '1';
            end if;
        end if;
    end process;


end Behavioral;

