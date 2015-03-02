#!/usr/bin/env ruby
#
# A Ruby implementation of the same logic.
#

ret = 1

if(ARGV.length < 1)
	puts "#{__FILE__} <DATA_FILE> ..."
else
	counts = {}
	files = ARGV
	files.each do |name|
		# puts "Processing file #{name}..."
		f = File.open(name)
		f.each_with_index do |line, i|
			off = i % 4
			if off == 1
				snip = line[0..3]
				# puts snip
				if counts[snip]
					counts[snip] = counts[snip] + 1					
				else
					counts[snip] = 1
				end
			end
		end
		
	end
	# puts ARGV[1]
	ret = 0
	puts "Prefix, Count"
	counts.each do |k, v|
		puts "#{k}, #{v}"
	end
end


exit ret