LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;

Entity two_pt_butterfly is

port (
	x_re : inout std_logic_vector(7 downto 0); 
	y_re : inout std_logic_vector(7 downto 0); 
	x_im:  inout std_logic_vector(7 downto 0); 
	y_im : inout std_logic_vector(7 downto 0); 
	w_re : inout std_logic_vector(7 downto 0);
	w_im : inout std_logic_vector(7 downto 0);
	A_re : out std_logic_vector(7 downto 0);
	A_im : out std_logic_vector(7 downto 0);
	B_re : out std_logic_vector(7 downto 0);
	B_im : out std_logic_vector(7 downto 0) 
);
end two_pt_butterfly;

architecture behavioral of two_pt_butterfly is
 signal temp1, temp2 : std_logic_vector(15 downto 0);

begin
	temp1 <= (y_re*w_re) - (y_im*w_im);
	temp2 <= (y_re*w_im) + (y_im*w_re);

	A_re <= x_re + temp1(7 downto 0);
	A_im <= x_im + temp2(7 downto 0);

	B_re <= x_re - temp1(7 downto 0);
	B_im <= x_im - temp2(7 downto 0);

	DUT:process

	begin
		x_re <="00000110";
		y_re <="00000101";
	
		x_im <="00000010";
		y_im <="00000111";

		w_re <="00000001";
		w_im <="00000000";
		
		wait;
	end process DUT;

end architecture;
