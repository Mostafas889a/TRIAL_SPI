`timescale 1ns / 1ps
`default_nettype none

module user_project (
`ifdef USE_POWER_PINS
    inout vccd1,
    inout vssd1,
`endif
    input wire wb_clk_i,
    input wire wb_rst_i,
    input wire wbs_stb_i,
    input wire wbs_cyc_i,
    input wire wbs_we_i,
    input wire [3:0] wbs_sel_i,
    input wire [31:0] wbs_dat_i,
    input wire [31:0] wbs_adr_i,
    output wire wbs_ack_o,
    output wire [31:0] wbs_dat_o,
    output wire [2:0] user_irq,
    input wire [1:0] spi_miso,
    output wire [1:0] spi_mosi,
    output wire [1:0] spi_csb,
    output wire [1:0] spi_sclk
);

  wire periph_sel;
  wire [1:0] stb;
  wire [1:0] ack;
  wire [31:0] dat_o [1:0];
  wire [1:0] irq;

  assign periph_sel = wbs_adr_i[16];
  assign stb[0] = ~periph_sel & wbs_stb_i;
  assign stb[1] = periph_sel & wbs_stb_i;

  assign wbs_ack_o = ack[0] | ack[1];

  reg [31:0] mux_dat_o;
  always @(*) begin
    case (periph_sel)
      1'b0: mux_dat_o = dat_o[0];
      1'b1: mux_dat_o = dat_o[1];
      default: mux_dat_o = 32'hDEADBEEF;
    endcase
  end
  assign wbs_dat_o = mux_dat_o;

  assign user_irq[0] = irq[0];
  assign user_irq[1] = irq[1];
  assign user_irq[2] = 1'b0;

  CF_SPI_WB #(
      .CDW(8),
      .FAW(4)
  ) spi0 (
      .clk_i (wb_clk_i),
      .rst_i (wb_rst_i),
      .adr_i (wbs_adr_i),
      .dat_i (wbs_dat_i),
      .dat_o (dat_o[0]),
      .sel_i (wbs_sel_i),
      .cyc_i (wbs_cyc_i),
      .stb_i (stb[0]),
      .ack_o (ack[0]),
      .we_i  (wbs_we_i),
      .IRQ   (irq[0]),
      .miso  (spi_miso[0]),
      .mosi  (spi_mosi[0]),
      .csb   (spi_csb[0]),
      .sclk  (spi_sclk[0])
  );

  CF_SPI_WB #(
      .CDW(8),
      .FAW(4)
  ) spi1 (
      .clk_i (wb_clk_i),
      .rst_i (wb_rst_i),
      .adr_i (wbs_adr_i),
      .dat_i (wbs_dat_i),
      .dat_o (dat_o[1]),
      .sel_i (wbs_sel_i),
      .cyc_i (wbs_cyc_i),
      .stb_i (stb[1]),
      .ack_o (ack[1]),
      .we_i  (wbs_we_i),
      .IRQ   (irq[1]),
      .miso  (spi_miso[1]),
      .mosi  (spi_mosi[1]),
      .csb   (spi_csb[1]),
      .sclk  (spi_sclk[1])
  );

endmodule

`default_nettype wire
