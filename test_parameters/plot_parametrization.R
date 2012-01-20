cmdargs=commandArgs(TRUE);

log_mode=FALSE;
if(length(cmdargs)==2)
{
  if(cmdargs[2]=="log")
  {
    log_mode=TRUE;
  }
}

t=read.table(cmdargs[1], header=FALSE, stringsAsFactors=FALSE);
p=t[[2]][which(t[[1]]=="param")];
v=t[[2]][which(t[[1]]=="user")];
if(log_mode==TRUE)
{
  p=log(p);
}
png(paste(cmdargs[1], ".png", sep=""), width=7, height=7, units="in", res=300);
plot(x=p, y=v, col="black", type="l", main="Parameter vs Time", xlab="Parameter", ylab="Time");
dev.off();
