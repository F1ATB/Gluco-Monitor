Import("env")

env.Replace(PROGNAME="Gluco-Monitor_%s" % env.GetProjectOption("custom_prog_version"))