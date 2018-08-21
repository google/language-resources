tokenize_and_classify.far: tokenize_and_classify.grm classify.far universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

classify.far: classify.grm address.far citation.far date.far digit.far lseq.far mapper.far measure.far ordinal.far range.far telephone.far time.far salutation.far universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

address.far: address.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

universal_rules.far: universal_rules.grm 
	thraxcompiler --input_grammar=$< --output_far=$@

universal_func.far: universal_func.grm universal_rules.far
	thraxcompiler --input_grammar=$< --output_far=$@

citation.far: citation.grm universal_rules.far universal_func.far bible_books.txt
	thraxcompiler --input_grammar=$< --output_far=$@

date.far: date.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

digit.far: digit.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

lseq.far: lseq.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

mapper.far: mapper.grm money.far universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

money.far: money.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

measure.far: measure.grm mapper.far universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

ordinal.far: ordinal.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

range.far: range.grm date.far lseq.far mapper.far measure.far time.far universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

time.far: time.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

telephone.far: telephone.grm universal_rules.far universal_func.far
	thraxcompiler --input_grammar=$< --output_far=$@

salutation.far: salutation.grm universal_rules.far
	thraxcompiler --input_grammar=$< --output_far=$@

clean:
	rm -f classify.far universal_rules.far universal_func.far address.far citation.far date.far digit.far lseq.far mapper.far measure.far ordinal.far range.far telephone.far time.far salutation.far money.far
