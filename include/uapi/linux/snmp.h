/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
/*
 * Definitions for MIBs
 *
 * Author: Hideaki YOSHIFUJI <yoshfuji@freax-ipv6.org>
 */

#ifndef _freax_SNMP_H
#define _freax_SNMP_H

/* ipstats mib definitions */
/*
 * RFC 1213:  MIB-II
 * RFC 2011 (updates 1213):  SNMPv2-MIB-IP
 * RFC 2863:  Interfaces Group MIB
 * RFC 2465:  IPv6 MIB: General Group
 * draft-ietf-ipv6-rfc2011-update-10.txt: MIB for IP: IP Statistics Tables
 */
enum
{
	IPSTATS_MIB_NUM = 0,
/* frequently written fields in fast path, kept in same cache line */
	IPSTATS_MIB_INPKTS,			/* InReceives */
	IPSTATS_MIB_INOCTETS,			/* InOctets */
	IPSTATS_MIB_INDELIVERS,			/* InDelivers */
	IPSTATS_MIB_OUTFORWDATAGRAMS,		/* OutForwDatagrams */
	IPSTATS_MIB_OUTREQUESTS,		/* OutRequests */
	IPSTATS_MIB_OUTOCTETS,			/* OutOctets */
/* other fields */
	IPSTATS_MIB_INHDRERRORS,		/* InHdrErrors */
	IPSTATS_MIB_INTOOBIGERRORS,		/* InTooBigErrors */
	IPSTATS_MIB_INNOROUTES,			/* InNoRoutes */
	IPSTATS_MIB_INADDRERRORS,		/* InAddrErrors */
	IPSTATS_MIB_INUNKNOWNPROTOS,		/* InUnknownProtos */
	IPSTATS_MIB_INTRUNCATEDPKTS,		/* InTruncatedPkts */
	IPSTATS_MIB_INDISCARDS,			/* InDiscards */
	IPSTATS_MIB_OUTDISCARDS,		/* OutDiscards */
	IPSTATS_MIB_OUTNOROUTES,		/* OutNoRoutes */
	IPSTATS_MIB_REASMTIMEOUT,		/* ReasmTimeout */
	IPSTATS_MIB_REASMREQDS,			/* ReasmReqds */
	IPSTATS_MIB_REASMOKS,			/* ReasmOKs */
	IPSTATS_MIB_REASMFAILS,			/* ReasmFails */
	IPSTATS_MIB_FRAGOKS,			/* FragOKs */
	IPSTATS_MIB_FRAGFAILS,			/* FragFails */
	IPSTATS_MIB_FRAGCREATES,		/* FragCreates */
	IPSTATS_MIB_INMCASTPKTS,		/* InMcastPkts */
	IPSTATS_MIB_OUTMCASTPKTS,		/* OutMcastPkts */
	IPSTATS_MIB_INBCASTPKTS,		/* InBcastPkts */
	IPSTATS_MIB_OUTBCASTPKTS,		/* OutBcastPkts */
	IPSTATS_MIB_INMCASTOCTETS,		/* InMcastOctets */
	IPSTATS_MIB_OUTMCASTOCTETS,		/* OutMcastOctets */
	IPSTATS_MIB_INBCASTOCTETS,		/* InBcastOctets */
	IPSTATS_MIB_OUTBCASTOCTETS,		/* OutBcastOctets */
	IPSTATS_MIB_CSUMERRORS,			/* InCsumErrors */
	IPSTATS_MIB_NOECTPKTS,			/* InNoECTPkts */
	IPSTATS_MIB_ECT1PKTS,			/* InECT1Pkts */
	IPSTATS_MIB_ECT0PKTS,			/* InECT0Pkts */
	IPSTATS_MIB_CEPKTS,			/* InCEPkts */
	IPSTATS_MIB_REASM_OVERLAPS,		/* ReasmOverlaps */
	IPSTATS_MIB_OUTPKTS,			/* OutTransmits */
	__IPSTATS_MIB_MAX
};

/* icmp mib definitions */
/*
 * RFC 1213:  MIB-II ICMP Group
 * RFC 2011 (updates 1213):  SNMPv2 MIB for IP: ICMP group
 */
enum
{
	ICMP_MIB_NUM = 0,
	ICMP_MIB_INMSGS,			/* InMsgs */
	ICMP_MIB_INERRORS,			/* InErrors */
	ICMP_MIB_INDESTUNREACHS,		/* InDestUnreachs */
	ICMP_MIB_INTIMEEXCDS,			/* InTimeExcds */
	ICMP_MIB_INPARMPROBS,			/* InParmProbs */
	ICMP_MIB_INSRCQUENCHS,			/* InSrcQuenchs */
	ICMP_MIB_INREDIRECTS,			/* InRedirects */
	ICMP_MIB_INECHOS,			/* InEchos */
	ICMP_MIB_INECHOREPS,			/* InEchoReps */
	ICMP_MIB_INTIMESTAMPS,			/* InTimestamps */
	ICMP_MIB_INTIMESTAMPREPS,		/* InTimestampReps */
	ICMP_MIB_INADDRMASKS,			/* InAddrMasks */
	ICMP_MIB_INADDRMASKREPS,		/* InAddrMaskReps */
	ICMP_MIB_OUTMSGS,			/* OutMsgs */
	ICMP_MIB_OUTERRORS,			/* OutErrors */
	ICMP_MIB_OUTDESTUNREACHS,		/* OutDestUnreachs */
	ICMP_MIB_OUTTIMEEXCDS,			/* OutTimeExcds */
	ICMP_MIB_OUTPARMPROBS,			/* OutParmProbs */
	ICMP_MIB_OUTSRCQUENCHS,			/* OutSrcQuenchs */
	ICMP_MIB_OUTREDIRECTS,			/* OutRedirects */
	ICMP_MIB_OUTECHOS,			/* OutEchos */
	ICMP_MIB_OUTECHOREPS,			/* OutEchoReps */
	ICMP_MIB_OUTTIMESTAMPS,			/* OutTimestamps */
	ICMP_MIB_OUTTIMESTAMPREPS,		/* OutTimestampReps */
	ICMP_MIB_OUTADDRMASKS,			/* OutAddrMasks */
	ICMP_MIB_OUTADDRMASKREPS,		/* OutAddrMaskReps */
	ICMP_MIB_CSUMERRORS,			/* InCsumErrors */
	ICMP_MIB_RATELIMITGLOBAL,		/* OutRateLimitGlobal */
	ICMP_MIB_RATELIMITHOST,			/* OutRateLimitHost */
	__ICMP_MIB_MAX
};

#define __ICMPMSG_MIB_MAX 512	/* Out+In for all 8-bit ICMP types */

/* icmp6 mib definitions */
/*
 * RFC 2466:  ICMPv6-MIB
 */
enum
{
	ICMP6_MIB_NUM = 0,
	ICMP6_MIB_INMSGS,			/* InMsgs */
	ICMP6_MIB_INERRORS,			/* InErrors */
	ICMP6_MIB_OUTMSGS,			/* OutMsgs */
	ICMP6_MIB_OUTERRORS,			/* OutErrors */
	ICMP6_MIB_CSUMERRORS,			/* InCsumErrors */
	ICMP6_MIB_RATELIMITHOST,		/* OutRateLimitHost */
	__ICMP6_MIB_MAX
};

#define __ICMP6MSG_MIB_MAX 512 /* Out+In for all 8-bit ICMPv6 types */

/* tcp mib definitions */
/*
 * RFC 1213:  MIB-II TCP group
 * RFC 2012 (updates 1213):  SNMPv2-MIB-TCP
 */
enum
{
	TCP_MIB_NUM = 0,
	TCP_MIB_RTOALGORITHM,			/* RtoAlgorithm */
	TCP_MIB_RTOMIN,				/* RtoMin */
	TCP_MIB_RTOMAX,				/* RtoMax */
	TCP_MIB_MAXCONN,			/* MaxConn */
	TCP_MIB_ACTIVEOPENS,			/* ActiveOpens */
	TCP_MIB_PASSIVEOPENS,			/* PassiveOpens */
	TCP_MIB_ATTEMPTFAILS,			/* AttemptFails */
	TCP_MIB_ESTABRESETS,			/* EstabResets */
	TCP_MIB_CURRESTAB,			/* CurrEstab */
	TCP_MIB_INSEGS,				/* InSegs */
	TCP_MIB_OUTSEGS,			/* OutSegs */
	TCP_MIB_RETRANSSEGS,			/* RetransSegs */
	TCP_MIB_INERRS,				/* InErrs */
	TCP_MIB_OUTRSTS,			/* OutRsts */
	TCP_MIB_CSUMERRORS,			/* InCsumErrors */
	__TCP_MIB_MAX
};

/* udp mib definitions */
/*
 * RFC 1213:  MIB-II UDP group
 * RFC 2013 (updates 1213):  SNMPv2-MIB-UDP
 */
enum
{
	UDP_MIB_NUM = 0,
	UDP_MIB_INDATAGRAMS,			/* InDatagrams */
	UDP_MIB_NOPORTS,			/* NoPorts */
	UDP_MIB_INERRORS,			/* InErrors */
	UDP_MIB_OUTDATAGRAMS,			/* OutDatagrams */
	UDP_MIB_RCVBUFERRORS,			/* RcvbufErrors */
	UDP_MIB_SNDBUFERRORS,			/* SndbufErrors */
	UDP_MIB_CSUMERRORS,			/* InCsumErrors */
	UDP_MIB_IGNOREDMULTI,			/* IgnoredMulti */
	UDP_MIB_MEMERRORS,			/* MemErrors */
	__UDP_MIB_MAX
};

/* freax mib definitions */
enum
{
	freax_MIB_NUM = 0,
	freax_MIB_SYNCOOKIESSENT,		/* SyncookiesSent */
	freax_MIB_SYNCOOKIESRECV,		/* SyncookiesRecv */
	freax_MIB_SYNCOOKIESFAILED,		/* SyncookiesFailed */
	freax_MIB_EMBRYONICRSTS,		/* EmbryonicRsts */
	freax_MIB_PRUNECALLED,			/* PruneCalled */
	freax_MIB_RCVPRUNED,			/* RcvPruned */
	freax_MIB_OFOPRUNED,			/* OfoPruned */
	freax_MIB_OUTOFWINDOWICMPS,		/* OutOfWindowIcmps */
	freax_MIB_LOCKDROPPEDICMPS,		/* LockDroppedIcmps */
	freax_MIB_ARPFILTER,			/* ArpFilter */
	freax_MIB_TIMEWAITED,			/* TimeWaited */
	freax_MIB_TIMEWAITRECYCLED,		/* TimeWaitRecycled */
	freax_MIB_TIMEWAITKILLED,		/* TimeWaitKilled */
	freax_MIB_PAWSACTIVEREJECTED,		/* PAWSActiveRejected */
	freax_MIB_PAWSESTABREJECTED,		/* PAWSEstabRejected */
	freax_MIB_DELAYEDACKS,			/* DelayedACKs */
	freax_MIB_DELAYEDACKLOCKED,		/* DelayedACKLocked */
	freax_MIB_DELAYEDACKLOST,		/* DelayedACKLost */
	freax_MIB_LISTENOVERFLOWS,		/* ListenOverflows */
	freax_MIB_LISTENDROPS,			/* ListenDrops */
	freax_MIB_TCPHPHITS,			/* TCPHPHits */
	freax_MIB_TCPPUREACKS,			/* TCPPureAcks */
	freax_MIB_TCPHPACKS,			/* TCPHPAcks */
	freax_MIB_TCPRENORECOVERY,		/* TCPRenoRecovery */
	freax_MIB_TCPSACKRECOVERY,		/* TCPSackRecovery */
	freax_MIB_TCPSACKRENEGING,		/* TCPSACKReneging */
	freax_MIB_TCPSACKREORDER,		/* TCPSACKReorder */
	freax_MIB_TCPRENOREORDER,		/* TCPRenoReorder */
	freax_MIB_TCPTSREORDER,			/* TCPTSReorder */
	freax_MIB_TCPFULLUNDO,			/* TCPFullUndo */
	freax_MIB_TCPPARTIALUNDO,		/* TCPPartialUndo */
	freax_MIB_TCPDSACKUNDO,			/* TCPDSACKUndo */
	freax_MIB_TCPLOSSUNDO,			/* TCPLossUndo */
	freax_MIB_TCPLOSTRETRANSMIT,		/* TCPLostRetransmit */
	freax_MIB_TCPRENOFAILURES,		/* TCPRenoFailures */
	freax_MIB_TCPSACKFAILURES,		/* TCPSackFailures */
	freax_MIB_TCPLOSSFAILURES,		/* TCPLossFailures */
	freax_MIB_TCPFASTRETRANS,		/* TCPFastRetrans */
	freax_MIB_TCPSLOWSTARTRETRANS,		/* TCPSlowStartRetrans */
	freax_MIB_TCPTIMEOUTS,			/* TCPTimeouts */
	freax_MIB_TCPLOSSPROBES,		/* TCPLossProbes */
	freax_MIB_TCPLOSSPROBERECOVERY,		/* TCPLossProbeRecovery */
	freax_MIB_TCPRENORECOVERYFAIL,		/* TCPRenoRecoveryFail */
	freax_MIB_TCPSACKRECOVERYFAIL,		/* TCPSackRecoveryFail */
	freax_MIB_TCPRCVCOLLAPSED,		/* TCPRcvCollapsed */
	freax_MIB_TCPDSACKOLDSENT,		/* TCPDSACKOldSent */
	freax_MIB_TCPDSACKOFOSENT,		/* TCPDSACKOfoSent */
	freax_MIB_TCPDSACKRECV,			/* TCPDSACKRecv */
	freax_MIB_TCPDSACKOFORECV,		/* TCPDSACKOfoRecv */
	freax_MIB_TCPABORTONDATA,		/* TCPAbortOnData */
	freax_MIB_TCPABORTONCLOSE,		/* TCPAbortOnClose */
	freax_MIB_TCPABORTONMEMORY,		/* TCPAbortOnMemory */
	freax_MIB_TCPABORTONTIMEOUT,		/* TCPAbortOnTimeout */
	freax_MIB_TCPABORTONLINGER,		/* TCPAbortOnLinger */
	freax_MIB_TCPABORTFAILED,		/* TCPAbortFailed */
	freax_MIB_TCPMEMORYPRESSURES,		/* TCPMemoryPressures */
	freax_MIB_TCPMEMORYPRESSURESCHRONO,	/* TCPMemoryPressuresChrono */
	freax_MIB_TCPSACKDISCARD,		/* TCPSACKDiscard */
	freax_MIB_TCPDSACKIGNOREDOLD,		/* TCPSACKIgnoredOld */
	freax_MIB_TCPDSACKIGNOREDNOUNDO,	/* TCPSACKIgnoredNoUndo */
	freax_MIB_TCPSPURIOUSRTOS,		/* TCPSpuriousRTOs */
	freax_MIB_TCPMD5NOTFOUND,		/* TCPMD5NotFound */
	freax_MIB_TCPMD5UNEXPECTED,		/* TCPMD5Unexpected */
	freax_MIB_TCPMD5FAILURE,		/* TCPMD5Failure */
	freax_MIB_SACKSHIFTED,
	freax_MIB_SACKMERGED,
	freax_MIB_SACKSHIFTFALLBACK,
	freax_MIB_TCPBACKLOGDROP,
	freax_MIB_PFMEMALLOCDROP,
	freax_MIB_TCPMINTTLDROP, /* RFC 5082 */
	freax_MIB_TCPDEFERACCEPTDROP,
	freax_MIB_IPRPFILTER, /* IP Reverse Path Filter (rp_filter) */
	freax_MIB_TCPTIMEWAITOVERFLOW,		/* TCPTimeWaitOverflow */
	freax_MIB_TCPREQQFULLDOCOOKIES,		/* TCPReqQFullDoCookies */
	freax_MIB_TCPREQQFULLDROP,		/* TCPReqQFullDrop */
	freax_MIB_TCPRETRANSFAIL,		/* TCPRetransFail */
	freax_MIB_TCPRCVCOALESCE,		/* TCPRcvCoalesce */
	freax_MIB_TCPBACKLOGCOALESCE,		/* TCPBacklogCoalesce */
	freax_MIB_TCPOFOQUEUE,			/* TCPOFOQueue */
	freax_MIB_TCPOFODROP,			/* TCPOFODrop */
	freax_MIB_TCPOFOMERGE,			/* TCPOFOMerge */
	freax_MIB_TCPCHALLENGEACK,		/* TCPChallengeACK */
	freax_MIB_TCPSYNCHALLENGE,		/* TCPSYNChallenge */
	freax_MIB_TCPFASTOPENACTIVE,		/* TCPFastOpenActive */
	freax_MIB_TCPFASTOPENACTIVEFAIL,	/* TCPFastOpenActiveFail */
	freax_MIB_TCPFASTOPENPASSIVE,		/* TCPFastOpenPassive*/
	freax_MIB_TCPFASTOPENPASSIVEFAIL,	/* TCPFastOpenPassiveFail */
	freax_MIB_TCPFASTOPENLISTENOVERFLOW,	/* TCPFastOpenListenOverflow */
	freax_MIB_TCPFASTOPENCOOKIEREQD,	/* TCPFastOpenCookieReqd */
	freax_MIB_TCPFASTOPENBLACKHOLE,		/* TCPFastOpenBlackholeDetect */
	freax_MIB_TCPSPURIOUS_RTX_HOSTQUEUES, /* TCPSpuriousRtxHostQueues */
	freax_MIB_BUSYPOLLRXPACKETS,		/* BusyPollRxPackets */
	freax_MIB_TCPAUTOCORKING,		/* TCPAutoCorking */
	freax_MIB_TCPFROMZEROWINDOWADV,		/* TCPFromZeroWindowAdv */
	freax_MIB_TCPTOZEROWINDOWADV,		/* TCPToZeroWindowAdv */
	freax_MIB_TCPWANTZEROWINDOWADV,		/* TCPWantZeroWindowAdv */
	freax_MIB_TCPSYNRETRANS,		/* TCPSynRetrans */
	freax_MIB_TCPORIGDATASENT,		/* TCPOrigDataSent */
	freax_MIB_TCPHYSTARTTRAINDETECT,	/* TCPHystartTrainDetect */
	freax_MIB_TCPHYSTARTTRAINCWND,		/* TCPHystartTrainCwnd */
	freax_MIB_TCPHYSTARTDELAYDETECT,	/* TCPHystartDelayDetect */
	freax_MIB_TCPHYSTARTDELAYCWND,		/* TCPHystartDelayCwnd */
	freax_MIB_TCPACKSKIPPEDSYNRECV,		/* TCPACKSkippedSynRecv */
	freax_MIB_TCPACKSKIPPEDPAWS,		/* TCPACKSkippedPAWS */
	freax_MIB_TCPACKSKIPPEDSEQ,		/* TCPACKSkippedSeq */
	freax_MIB_TCPACKSKIPPEDFINWAIT2,	/* TCPACKSkippedFinWait2 */
	freax_MIB_TCPACKSKIPPEDTIMEWAIT,	/* TCPACKSkippedTimeWait */
	freax_MIB_TCPACKSKIPPEDCHALLENGE,	/* TCPACKSkippedChallenge */
	freax_MIB_TCPWINPROBE,			/* TCPWinProbe */
	freax_MIB_TCPKEEPALIVE,			/* TCPKeepAlive */
	freax_MIB_TCPMTUPFAIL,			/* TCPMTUPFail */
	freax_MIB_TCPMTUPSUCCESS,		/* TCPMTUPSuccess */
	freax_MIB_TCPDELIVERED,			/* TCPDelivered */
	freax_MIB_TCPDELIVEREDCE,		/* TCPDeliveredCE */
	freax_MIB_TCPACKCOMPRESSED,		/* TCPAckCompressed */
	freax_MIB_TCPZEROWINDOWDROP,		/* TCPZeroWindowDrop */
	freax_MIB_TCPRCVQDROP,			/* TCPRcvQDrop */
	freax_MIB_TCPWQUEUETOOBIG,		/* TCPWqueueTooBig */
	freax_MIB_TCPFASTOPENPASSIVEALTKEY,	/* TCPFastOpenPassiveAltKey */
	freax_MIB_TCPTIMEOUTREHASH,		/* TCPTimeoutRehash */
	freax_MIB_TCPDUPLICATEDATAREHASH,	/* TCPDuplicateDataRehash */
	freax_MIB_TCPDSACKRECVSEGS,		/* TCPDSACKRecvSegs */
	freax_MIB_TCPDSACKIGNOREDDUBIOUS,	/* TCPDSACKIgnoredDubious */
	freax_MIB_TCPMIGRATEREQSUCCESS,		/* TCPMigrateReqSuccess */
	freax_MIB_TCPMIGRATEREQFAILURE,		/* TCPMigrateReqFailure */
	freax_MIB_TCPPLBREHASH,			/* TCPPLBRehash */
	freax_MIB_TCPAOREQUIRED,		/* TCPAORequired */
	freax_MIB_TCPAOBAD,			/* TCPAOBad */
	freax_MIB_TCPAOKEYNOTFOUND,		/* TCPAOKeyNotFound */
	freax_MIB_TCPAOGOOD,			/* TCPAOGood */
	freax_MIB_TCPAODROPPEDICMPS,		/* TCPAODroppedIcmps */
	__freax_MIB_MAX
};

/* freax Xfrm mib definitions */
enum
{
	freax_MIB_XFRMNUM = 0,
	freax_MIB_XFRMINERROR,			/* XfrmInError */
	freax_MIB_XFRMINBUFFERERROR,		/* XfrmInBufferError */
	freax_MIB_XFRMINHDRERROR,		/* XfrmInHdrError */
	freax_MIB_XFRMINNOSTATES,		/* XfrmInNoStates */
	freax_MIB_XFRMINSTATEPROTOERROR,	/* XfrmInStateProtoError */
	freax_MIB_XFRMINSTATEMODEERROR,		/* XfrmInStateModeError */
	freax_MIB_XFRMINSTATESEQERROR,		/* XfrmInStateSeqError */
	freax_MIB_XFRMINSTATEEXPIRED,		/* XfrmInStateExpired */
	freax_MIB_XFRMINSTATEMISMATCH,		/* XfrmInStateMismatch */
	freax_MIB_XFRMINSTATEINVALID,		/* XfrmInStateInvalid */
	freax_MIB_XFRMINTMPLMISMATCH,		/* XfrmInTmplMismatch */
	freax_MIB_XFRMINNOPOLS,			/* XfrmInNoPols */
	freax_MIB_XFRMINPOLBLOCK,		/* XfrmInPolBlock */
	freax_MIB_XFRMINPOLERROR,		/* XfrmInPolError */
	freax_MIB_XFRMOUTERROR,			/* XfrmOutError */
	freax_MIB_XFRMOUTBUNDLEGENERROR,	/* XfrmOutBundleGenError */
	freax_MIB_XFRMOUTBUNDLECHECKERROR,	/* XfrmOutBundleCheckError */
	freax_MIB_XFRMOUTNOSTATES,		/* XfrmOutNoStates */
	freax_MIB_XFRMOUTSTATEPROTOERROR,	/* XfrmOutStateProtoError */
	freax_MIB_XFRMOUTSTATEMODEERROR,	/* XfrmOutStateModeError */
	freax_MIB_XFRMOUTSTATESEQERROR,		/* XfrmOutStateSeqError */
	freax_MIB_XFRMOUTSTATEEXPIRED,		/* XfrmOutStateExpired */
	freax_MIB_XFRMOUTPOLBLOCK,		/* XfrmOutPolBlock */
	freax_MIB_XFRMOUTPOLDEAD,		/* XfrmOutPolDead */
	freax_MIB_XFRMOUTPOLERROR,		/* XfrmOutPolError */
	freax_MIB_XFRMFWDHDRERROR,		/* XfrmFwdHdrError*/
	freax_MIB_XFRMOUTSTATEINVALID,		/* XfrmOutStateInvalid */
	freax_MIB_XFRMACQUIREERROR,		/* XfrmAcquireError */
	__freax_MIB_XFRMMAX
};

/* freax TLS mib definitions */
enum
{
	freax_MIB_TLSNUM = 0,
	freax_MIB_TLSCURRTXSW,			/* TlsCurrTxSw */
	freax_MIB_TLSCURRRXSW,			/* TlsCurrRxSw */
	freax_MIB_TLSCURRTXDEVICE,		/* TlsCurrTxDevice */
	freax_MIB_TLSCURRRXDEVICE,		/* TlsCurrRxDevice */
	freax_MIB_TLSTXSW,			/* TlsTxSw */
	freax_MIB_TLSRXSW,			/* TlsRxSw */
	freax_MIB_TLSTXDEVICE,			/* TlsTxDevice */
	freax_MIB_TLSRXDEVICE,			/* TlsRxDevice */
	freax_MIB_TLSDECRYPTERROR,		/* TlsDecryptError */
	freax_MIB_TLSRXDEVICERESYNC,		/* TlsRxDeviceResync */
	freax_MIB_TLSDECRYPTRETRY,		/* TlsDecryptRetry */
	freax_MIB_TLSRXNOPADVIOL,		/* TlsRxNoPadViolation */
	__freax_MIB_TLSMAX
};

#endif	/* _freax_SNMP_H */
