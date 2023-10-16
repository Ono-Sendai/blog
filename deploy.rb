require 'pathname'

def exec_command(cmd)
	res = Kernel.system(cmd)
	if !res
		STDERR.puts "Command failed: " + cmd
		exit(1)
	end
end



def copy_file(src, dest)
	exec_command("pscp \"#{src}\" \"nick@forwardscattering.org:/home/nick/#{dest}\"")
end

#-P 18549


#copy_file("CMakeLists.txt", "website/")

#srcfiles = Dir.glob("src/*")
#srcfiles.each do |f|
#	copy_file(f, "website/" + f)
#end

indigo_trunk_dir = ENV["INDIGO_TRUNK_DIR"]
puts "indigo_trunk_dir: #{indigo_trunk_dir}"
raise "indigo_trunk_dir not found." if !indigo_trunk_dir
	

exec_command("sevenz a src.zip src")
exec_command("sevenz a src.zip systemd")
exec_command("sevenz a src.zip CMakeLists.txt")
exec_command("sevenz a src.zip N:/website_core/trunk")
exec_command("sevenz a src.zip #{indigo_trunk_dir}/double-conversion")
exec_command("sevenz a src.zip #{indigo_trunk_dir}/utils")
exec_command("sevenz a src.zip #{indigo_trunk_dir}/networking")
exec_command("sevenz a src.zip #{indigo_trunk_dir}/maths/mathstypes.h")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/maths/SSE.cpp")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/maths/SSE.h")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/indigo/TestUtils.h")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/indigo/globals.h")
copy_file("src.zip", "website/src.zip")

networking_files = Dir.glob("#{indigo_trunk_dir}/networking/*")

networking_files.each do |path|
	filename = Pathname.new(path).basename
	#copy_file(path, "website/indigo/networking/" + filename.to_s)
end

util_files = Dir.glob("#{indigo_trunk_dir}/utils/*")

util_files.each do |path|
	filename = Pathname.new(path).basename
	#copy_file(path, "website/indigo/utils/" + filename.to_s)
end

double_conversion_files = Dir.glob("#{indigo_trunk_dir}/double-conversion/*")

double_conversion_files.each do |path|
	filename = Pathname.new(path).basename
	#copy_file(path, "website/indigo/double-conversion/" + filename.to_s)
end