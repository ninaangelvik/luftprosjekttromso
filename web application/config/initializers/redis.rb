uri = 'redis://redistogo:f5f1eab9e222ade645fbaf18b5366698@porgy.redistogo.com:10033/'
REDIS = Redis.new(:url => uri)
Resque.redis = REDIS