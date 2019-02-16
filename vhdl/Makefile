TOPLEVEL = gc_ipl
PART = xc9572xl-10-VQ44

SOURCES = $(TOPLEVEL).vhd
UCF = $(TOPLEVEL).ucf

OUT = build
CLEAN = $(OUT) xlnx_auto_0_xdb _xmsgs _impactbatch.log

# Path to Xilinx tools, blank if in $PATH, must end in /
XILINX = /opt/Xilinx/14.7/ISE_DS/ISE/bin/lin64/

# Default flags selected by ISE, copied verbatim
XSTFLAGS = -opt_mode Speed \
    -opt_level 1 \
    -iuc NO \
    -keep_hierarchy Yes \
    -netlist_hierarchy As_Optimized \
    -rtlview Yes \
    -hierarchy_separator / \
    -bus_delimiter \<\> \
    -case Maintain \
    -verilog2001 YES \
    -fsm_extract YES -fsm_encoding Auto \
    -safe_implementation No \
    -mux_extract Yes \
    -resource_sharing YES \
    -iobuf YES \
    -pld_mp YES \
    -pld_xp YES \
    -pld_ce YES \
    -wysiwyg NO \
    -equivalent_register_removal YES

CPLDFITFLAGS = -optimize speed -slew fast -power std -init low -unused float -terminate keeper

ifdef V
    Q =
    E = @:
else
    Q = @
    E = @echo
endif

.PHONY: all clean run
.SECONDARY:

jed: $(OUT)/$(TOPLEVEL).jed

%.prj: $(SOURCES)
	$(E) "  PRJ      $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)rm -f $@
	$(Q)for i in $^; do \
	    echo "vhdl $(TOPLEVEL) $$i" >> $@; \
	done

%.lso:
	$(E) "  LSO      $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)echo "DEFAULT_SEARCH_ORDER" > $@

%.xst:
	$(E) "  XSTFILE  $@"
	$(Q)mkdir -p $(dir $@)xst
	$(Q)echo "set -tmpdir $(dir $@)xst -xsthdpdir $(dir $@)xst" > $@
	$(Q)echo run \
	    -ifn $(@:%.xst=%.prj) \
	    -ifmt mixed \
	    -ofn $(@:%.xst=%.ngc) \
	    -ofmt NGC \
	    -p $(PART) \
	    -top $(TOPLEVEL) \
	    -lso $(@:%.xst=%.lso) \
	    $(XSTFLAGS) >> $@

%.ngc: %.xst %.prj %.lso
	$(E) "  XST      $@"
	$(Q)$(XILINX)xst -ifn $< -ofn $(@:%.ngc=%.srp)

%.ngd: %.ngc $(UCF)
	$(E) "  NGDBUILD $@"
	$(Q)mkdir -p $(dir $@)ngo
	$(Q)$(XILINX)ngdbuild -dd $(dir $@)ngo -uc $(UCF) -p $(PART) $< $@

%.vm6: %.ngd
	$(E) "  CPLDFIT  $@"
	$(Q)cd $(dir $<); $(XILINX)cpldfit -p $(PART) $(notdir $<)

$(OUT)/$(TOPLEVEL).tim: $(OUT)/$(TOPLEVEL).vm6
	cd $(OUT) ; $(XILINX)taengine -l ../$@ -detail -f $(TOPLEVEL) ../$<

%.jed: %.vm6
	$(E) "  HPREP6   $@"
	$(Q)cd $(dir $<); $(XILINX)hprep6 -i $(notdir $<)

IMPACTCMD = $(<:%.jed=%.cmd)
run: $(OUT)/$(TOPLEVEL).jed
	$(E) "  IMPACT   $<"
	$(Q)echo "setmode -bscan" > $(IMPACTCMD)
	$(Q)echo "setcable -p auto" >> $(IMPACTCMD)
	$(Q)echo "Identify -inferir" >> $(IMPACTCMD)
	$(Q)echo "identifyMPM" >> $(IMPACTCMD)
	$(Q)echo "assignFile -p 1 -file $<" >> $(IMPACTCMD)
	$(Q)echo "program -p 1 -e -v" >> $(IMPACTCMD)
	$(Q)echo "deleteDevice -position 1" >> $(IMPACTCMD)
	$(Q)echo "quit" >> $(IMPACTCMD)
	$(Q)$(XILINX)impact -batch $(IMPACTCMD)

clean:
	$(E) "  RM       $(CLEAN)"
	$(Q)rm -rf $(CLEAN)
