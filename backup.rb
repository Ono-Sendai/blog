#
# Copies a timestamped website.data from the forwardscattering.org server to the local backup dir.
#
#

require './script_utils.rb'


def printUsage()
	puts "Usage: backup.rb [arguments]"
	puts ""
end

def run_command(s)
	puts s
	exec_command(s)
end


#if(ARGV.length == 0)
#	printUsage()
#	exit(1)
#end

	
$user = ENV["USERNAME"]

arg_parser = ArgumentParser.new(ARGV)

arg_parser.options.each do |opt|
	if opt[0] == "--user" || opt[0] == "-u"
		$user = opt[1]
	elsif opt[0] == "--help" || opt[0] == "-h"
		printUsage()
		exit 0
	else
		STDERR.puts "Unrecognised argument: #{opt[0]}"
		exit 1
	end
end

if $user == nil
	STDERR.puts "Error: Username must be specified."
	exit 1
end


# -batch removes interactive prompt.
ssh_program = "plink -P 22 -batch" if OS.windows?
ssh_program = "ssh" if !OS.windows?

suffix = "#{Time.now.year}_#{Time.now.month}_#{Time.now.day}__#{Time.now.hour}_#{Time.now.min}"
timestamped_filename = "website_#{suffix}.data"
	
run_command("#{ssh_program} #{$user}@forwardscattering.org \"cp /home/nick/website/rundir/test_builds/website.data /home/nick/website/rundir/test_builds/#{timestamped_filename}") # Copy server state bin to timestamped file

run_command("#{ssh_program} #{$user}@forwardscattering.org \"gzip /home/nick/website/rundir/test_builds/#{timestamped_filename}") # GZip it
	
run_command("pscp -P 22 #{$user}@forwardscattering.org:/home/nick/website/rundir/test_builds/#{timestamped_filename}.gz c:/programming/blog/backup/#{timestamped_filename}.gz") # download gzipped file


# TODO: back up stuff in public files
