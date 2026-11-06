#
# Backs up the blog from the forwardscattering.org server to the local backup dir:
#   * website.data (the DB), as a timestamped .data.gz
#   * blog_public_files (images, videos etc.), as a timestamped .tar
#
# Usage: ruby backup.rb [--user nick] [--dir c:/code/blog/backup]
#

require './script_utils.rb'


def printUsage()
	puts "Usage: backup.rb [arguments]"
	puts ""
	puts "  --user, -u    Username on server (default: USERNAME env var)"
	puts "  --dir, -d     Local backup dir (default: backup dir next to this script)"
	puts ""
end

def run_command(s)
	puts s
	exec_command(s)
end


$user = ENV["USERNAME"]
$backup_dir = File.join(File.dirname(File.expand_path(__FILE__)), "backup")

arg_parser = ArgumentParser.new(ARGV)

arg_parser.options.each do |opt|
	if opt[0] == "--user" || opt[0] == "-u"
		$user = opt[1]
	elsif opt[0] == "--dir" || opt[0] == "-d"
		$backup_dir = opt[1]
	elsif opt[0] == "--help" || opt[0] == "-h"
		printUsage()
		exit 0
	else
		STDERR.puts "Unrecognised argument: #{opt[0]}"
		exit 1
	end
end

if $user == nil || $backup_dir == nil
	STDERR.puts "Error: user and dir must be specified."
	exit 1
end


# -batch removes interactive prompt.
ssh_program = OS.windows? ? "plink -P 22 -batch" : "ssh"
scp_program = OS.windows? ? "pscp -P 22 -batch" : "scp -P 22"

host = "forwardscattering.org"

suffix = Time.now.strftime("%Y_%m_%d__%H_%M")

db_filename  = "website_#{suffix}.data.gz"
tar_filename = "blog_public_files_#{suffix}.tar"

remote_db_path  = "/tmp/#{db_filename}"
remote_tar_path = "/tmp/#{tar_filename}"

Dir.mkdir($backup_dir) if !Dir.exist?($backup_dir)

begin
	# GZip the DB into a temp file on the server.  (website.data is written atomically with a rename, so reading it while the server is running is fine.)
	run_command("#{ssh_program} #{$user}@#{host} \"gzip -c /home/nick/website/rundir/test_builds/website.data > #{remote_db_path}\"")

	# Tar the public files on the server.  Not compressed as the files are mostly jpg/png/mp4 which are already compressed.
	run_command("#{ssh_program} #{$user}@#{host} \"tar -C /home/nick -cf #{remote_tar_path} blog_public_files\"")

	# Download both
	run_command("#{scp_program} #{$user}@#{host}:#{remote_db_path} \"#{File.join($backup_dir, db_filename)}\"")
	run_command("#{scp_program} #{$user}@#{host}:#{remote_tar_path} \"#{File.join($backup_dir, tar_filename)}\"")
ensure
	# Remove the temp files from the server, even if something failed.
	exec_command_no_exit("#{ssh_program} #{$user}@#{host} \"rm -f #{remote_db_path} #{remote_tar_path}\"")
end

[db_filename, tar_filename].each do |f|
	path = File.join($backup_dir, f)
	puts "Backed up to #{path} (#{'%.1f' % (File.size(path) / (1024.0 * 1024.0))} MB)"
end
