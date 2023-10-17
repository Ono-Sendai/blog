require 'pathname'
require './script_utils.rb'



def copy_file(src, dest)
	exec_command("pscp \"#{src}\" \"nick@forwardscattering.org:/home/nick/#{dest}\"")
end

#-P 18549


#copy_file("CMakeLists.txt", "website/")

#srcfiles = Dir.glob("src/*")
#srcfiles.each do |f|
#	copy_file(f, "website/" + f)
#end

glare_core_trunk_dir = ENV["GLARE_CORE_TRUNK_DIR"]
puts "glare_core_trunk_dir: #{glare_core_trunk_dir}"
raise "glare_core_trunk_dir not found." if !glare_core_trunk_dir
	

exec_command("sevenz a src.zip src")
exec_command("sevenz a src.zip systemd")
exec_command("sevenz a src.zip CMakeLists.txt")
exec_command("sevenz a src.zip #{glare_core_trunk_dir}/webserver")
exec_command("sevenz a src.zip #{glare_core_trunk_dir}/double-conversion")
exec_command("sevenz a src.zip #{glare_core_trunk_dir}/utils")
exec_command("sevenz a src.zip #{glare_core_trunk_dir}/networking")
exec_command("sevenz a src.zip #{glare_core_trunk_dir}/maths/mathstypes.h")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/maths/SSE.cpp")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/maths/SSE.h")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/indigo/TestUtils.h")
#exec_command("sevenz a src.zip #{indigo_trunk_dir}/indigo/globals.h")
copy_file("src.zip", "website/src.zip")

networking_files = Dir.glob("#{glare_core_trunk_dir}/networking/*")

