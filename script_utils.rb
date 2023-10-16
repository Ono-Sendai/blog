#
# Various utility methods for scripting.
#
# Copyright Glare Technologies Limited 2023 -
#

require 'rubygems'
require 'open3'
require 'net/http'


def exec_command(cmd)
	res = Kernel.system(cmd)
	if !res
		STDERR.puts "Command failed: " + cmd
		exit(1)
	end
end


def exec_command_no_exit(cmd)
	res = Kernel.system(cmd)
	if !res
		STDERR.puts "Command failed: " + cmd
		return 1
	end

	return 0
end


def print_and_exec_command(cmd)
	puts "------Executing command------"
	puts cmd
	puts "-----------------------------"

	exec_command(cmd)
end







module OS
	def OS.windows?
		(/cygwin|mswin|mingw|bccwin|wince|emx/ =~ RUBY_PLATFORM) != nil
	end

	def OS.mac?
		(/darwin/ =~ RUBY_PLATFORM) != nil
	end

	def OS.arm64?
		(/arm64/ =~ RUBY_PLATFORM) != nil
	end

	def OS.unix?
		!OS.windows?
	end

	def OS.linux?
		OS.unix? and not OS.mac?
	end

	def OS.linux64?
		OS.linux? and (/64/ =~ RUBY_PLATFORM) != nil
	end
end


# A simple argument parser.
# It handles:
#	* long args "--xxxx" also with values "--xxxxx yyyyyy"
#	* short args "-x" also with values "-x yyyyyy"
#	* lists of short args "-xxxxx" without values
#
# It does not handle unnamed arguments!
class ArgumentParser
	attr_accessor :options, :input

	# An array of string argumenst. ARGV, for example.
	def initialize(args_in)
		@input = args_in
		@options = []
		parse()
	end

	private # All private from here

	def starts_with?(string, characters)
		string.match(/^#{characters}/) ? true : false
	end


	def nextVal(idx)
		next_arg = @input[idx+1]
		if next_arg != nil
			if !starts_with?(next_arg, "-")
				return next_arg
			end
		end

		return nil
	end


	def addToOptions(arg, value)
		if arg != nil
			option = [arg, value]

			@options.push(option)
		end
	end


	def parse()
		i = 0
		while i < @input.length
			current_arg = @input[i]
			#puts current_arg

			if starts_with?(current_arg, "--") # if its a long arg
				arg = current_arg

				value = nextVal(i)

				if value != nil
					i = i + 1
				end

				addToOptions(arg, value)
			elsif starts_with?(current_arg, "-") && current_arg.length == 2 # A short arg
				arg = current_arg

				value = nextVal(i)
				if value != nil
					i = i + 1
				end

				addToOptions(arg, value)
			elsif starts_with?(current_arg, "-") && current_arg.length > 2 # A list of short args. Note: lists of short args can't have values.
				args = current_arg[1..-1]

				args.split("").each do |j|
					addToOptions("-#{j}", nil)
				end

			elsif !starts_with?(current_arg, "-") # Unnamed arg. Doesn't support unnamed. Raise.
				raise ArgumentError, "Found unnamed argument: '#{current_arg}'"
			else
				raise ArgumentError, "What the hell are you doing? ... #{current_arg}"
				puts "Error!"
			end

			i = i + 1
		end
	end
end


def getAndCheckEnvVar(name)
	env_var = ENV[name]

	if env_var.nil?
		STDERR.puts "#{name} env var not defined."
		exit(1)
	end

	puts "#{name}: #{env_var}"
	return env_var
end


class Timer
	def self.time(&block)
		start_time = Time.now
		result = block.call
		end_time = Time.now
		@time_taken = end_time - start_time
		result
	end

	def self.elapsedTime
		return @time_taken
	end

end
