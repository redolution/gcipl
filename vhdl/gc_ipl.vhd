library ieee;
use ieee.std_logic_1164.all;

entity gc_ipl is
    port (
        exi_miso : out std_logic_vector(3 downto 0);
        exi_mosi : in std_logic;
        exi_cs : in std_logic;
        exi_clk : in std_logic;

        f_miso : in std_logic;
        f_mosi : out std_logic;
        f_cs : out std_logic;
        f_clk : out std_logic
    );
end gc_ipl;

architecture Behavioral of gc_ipl is
    type state_t is (
        translate,
        passthrough_wait,
        passthrough,
        ignore
    );
    signal state : state_t;

    signal outbuf : std_logic_vector(4 downto 0);
    signal bits : integer range 0 to 31;
begin
    f_clk <= 'Z'; -- This pin needs to go, clock should be routed directly on the PCB
    f_cs <= exi_cs when (state = translate or state = passthrough) else '1';
    exi_miso <= (others => f_miso) when (state /= ignore and exi_cs = '0') else (others => 'Z');

    process (exi_cs, exi_clk)
    begin
        if exi_cs = '1' then
            state <= translate;
            outbuf <= (others => '0');
            bits <= 0;
            f_mosi <= '0';
        elsif rising_edge(exi_clk) then
            case state is
                when translate | passthrough_wait =>
                    f_mosi <= outbuf(4);
                    outbuf <= outbuf(3 downto 0) & exi_mosi;
                    bits <= bits + 1;

                    case bits is
                        when 0 =>
                            outbuf <= outbuf(3 downto 0) & '1';
                            -- When writing, temporarily deselect flash
                            -- It will be reselected later on, so that raw commands can be issued
                            if exi_mosi = '1' then
                                state <= passthrough_wait;
                            end if;

                        when 1 =>
                            outbuf <= outbuf(3 downto 0) & '1';

                        when 2 | 3 | 4 | 5 | 6 | 7 =>
                            -- Flash is only 512KB, no point in overriding accesses higher than that
                            -- Let it all fall through to the IPL ROM (for fonts) and other peripherals
                            if exi_mosi = '1' then
                                state <= ignore;
                            end if;

                        when 31 =>
                            state <= passthrough;

                        when others =>
                            null;
                    end case;

                when passthrough =>
                    f_mosi <= exi_mosi;

                when ignore =>
                    null;
            end case;
        end if;
    end process;
end Behavioral;
