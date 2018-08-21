verbalize.far: verbalize.grm universal_rules.far universal_func.far cardinal.far date.far decimal.far digit.far electronic.far fraction.far lseq.far measure.far money.far ordinal.far telephone.far time.far salutation.far verbatim.far
	thraxcompiler --input_grammar=$< --output_far=$@

universal_rules.far: universal_rules.grm 
	thraxcompiler --input_grammar=$< --output_far=$@

universal_func.far: universal_func.grm universal_rules.far
	thraxcompiler --input_grammar=$< --output_far=$@

cardinal.far: cardinal.grm number_names_rules.far universal_rules.far universal_func.far digit.far
	thraxcompiler --input_grammar=$< --output_far=$@

number_names_rules.far: number_names_rules.grm share_grms/numbers.far
	thraxcompiler --input_grammar=$< --output_far=$@

share_grms/numbers.far: share_grms/numbers.grm share_grms/byte.far
	thraxcompiler --input_grammar=$< --output_far=$@

share_grms/byte.far: share_grms/byte.grm 
	thraxcompiler --input_grammar=$< --output_far=$@

digit.far: digit.grm number_names_rules.far universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

date.far: date.grm universal_rules.far universal_func.far cardinal.far
	thraxcompiler --input_grammar=$< --output_far=$@

decimal.far: decimal.grm universal_rules.far universal_func.far cardinal.far digit.far ordinal.far
	thraxcompiler --input_grammar=$< --output_far=$@

ordinal.far: ordinal.grm number_names_rules.far universal_rules.far universal_func.far cardinal.far
	thraxcompiler --input_grammar=$< --output_far=$@

electronic.far: electronic.grm universal_rules.far universal_func.far cardinal.far date.far verbatim.far
	thraxcompiler --input_grammar=$< --output_far=$@

verbatim.far: verbatim.grm universal_rules.far universal_func.far digit.far lseq.far verbatim_mappings.txt
	thraxcompiler --input_grammar=$< --output_far=$@

lseq.far: lseq.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

fraction.far: fraction.grm universal_rules.far universal_func.far cardinal.far ordinal.far
	thraxcompiler --input_grammar=$< --output_far=$@

measure.far: measure.grm universal_rules.far universal_func.far cardinal.far decimal.far fraction.far
	thraxcompiler --input_grammar=$< --output_far=$@

money.far: money.grm universal_rules.far universal_func.far cardinal.far decimal.far
	thraxcompiler --input_grammar=$< --output_far=$@

telephone.far: telephone.grm universal_rules.far universal_func.far cardinal.far verbatim.far
	thraxcompiler --input_grammar=$< --output_far=$@

time.far: time.grm universal_rules.far universal_func.far cardinal.far verbatim.far
	thraxcompiler --input_grammar=$< --output_far=$@

salutation.far: salutation.grm universal_rules.far
	thraxcompiler --input_grammar=$< --output_far=$@

clean:
	rm -f universal_rules.far universal_func.far cardinal.far date.far decimal.far digit.far electronic.far fraction.far lseq.far measure.far money.far ordinal.far telephone.far time.far salutation.far verbatim.far number_names_rules.far share_grms/numbers.far share_grms/byte.far
