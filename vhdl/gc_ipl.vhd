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
        ignore,
        disable
    );
    signal state : state_t;

    signal outbuf : std_logic_vector(5 downto 0);
    signal zero : std_logic;
    signal one : std_logic;
    signal bits : integer range 0 to 31;
begin
    f_clk <= 'Z'; -- This pin needs to go, clock should be routed directly on the PCB
    f_cs <= exi_cs when (state = translate or state = passthrough) else '1';
    f_mosi <= outbuf(5) when state = translate else exi_mosi;
    exi_miso <= (others => f_miso) when (state /= ignore and state /= disable and exi_cs = '0') else (others => 'Z');

    process (exi_cs, exi_clk, state)
    begin
        if exi_cs = '1' then
            if state /= disable then
                state <= translate;
            end if;
            outbuf <= (others => '0');
            zero <= '0';
            one <= '1';
            bits <= 0;
        elsif rising_edge(exi_clk) then
            case state is
                when translate | passthrough_wait =>
                    outbuf <= outbuf(4 downto 0) & exi_mosi;
                    zero <= zero or exi_mosi;
                    one <= one and exi_mosi;
                    bits <= bits + 1;

                    case bits is
                        when 0 =>
                            outbuf(0) <= '1';
                            -- When writing, temporarily deselect flash
                            -- It will be reselected later on, so that raw commands can be issued
                            if exi_mosi = '1' then
                                state <= passthrough_wait;
                            end if;

                        when 1 =>
                            outbuf(0) <= '1';

                        when 2 | 3 | 4 | 5 | 6 =>
                            -- Flash is only 512KB, no point in overriding accesses higher than that
                            -- Let it all fall through to the IPL ROM (for fonts) and other peripherals
                            if exi_mosi = '1' then
                                state <= ignore;
                            end if;

                        when 26 =>
                            -- Ignore reads from address 0, to allow Swiss to read the copyright/version string
                            if zero = '0' then
                                state <= ignore;
                            end if;

                            if one = '1' then
                                state <= disable;
                            end if;

                        when 31 =>
                            state <= passthrough;

                        when others =>
                            null;
                    end case;

                when passthrough =>
                    null;

                when ignore | disable =>
                    null;
            end case;
        end if;
    end process;
end Behavioral;
