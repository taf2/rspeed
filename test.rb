require 'rubygems'
require 'curb'
require 'hpricot'
require 'speed_core'

c = Curl::Easy.new("http://muglazgo.com/")
c.follow_location = true
c.headers["Accept-Encoding"] = "gzip, deflate"
c.headers["Content-Encoding"] = "gzip"

c.perform

r = Speed::Resource.new(c.url, 'GET')
r.add_request({"Accept-Encoding" => "gzip, deflate",
               "Content-Type" => "text/html; charset=UTF8",
               "Content-Encoding" => "gzip"})
r.add_response(c.response_code, c.body_str)

input = Speed::Input.new
input.add(r)

engine = Speed::Engine.new(Speed::ALL)

result = engine.compute(input)
puts result.inspect
