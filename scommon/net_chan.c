#include "s_common.h"

cvar		*showpackets;
cvar		*showdrop;
cvar		*sport;

netadr_t	net_from;
buffer_t	net_message;
byte		net_message_buffer[MAX_MSGLEN];

/*
===============
Netchan_Init

===============
*/
void Netchan_Init (void)
{
	int		port;

	// pick a port value that should be nice and random
	port = OS_Milliseconds() & 0xffff;

	showpackets = Cvar_Get ("showpackets", "0", 0);
	showdrop = Cvar_Get ("showdrop", "0", 0);
	sport = Cvar_Get ("qport", va("%i", port), 0);
}

/*
===============
Netchan_OutOfBand

Sends an out-of-band datagram
================
*/
void Netchan_OutOfBand (int net_socket, netadr_t adr, int length, byte *data)
{
	buffer_t	send;
	byte		send_buf[MAX_MSGLEN];

// write the packet header
	Buf_Init (&send, send_buf, sizeof(send_buf));
	
	MSG_WriteLong (&send, -1);	// -1 sequence means out of band
	Buf_Write (&send, data, length);

// send the datagram
	NET_SendPacket (net_socket, send.currentsize, send.data, adr);
}

/*
===============
Netchan_OutOfBandPrint

Sends a text message in an out-of-band datagram
================
*/
void Netchan_OutOfBandPrint (int net_socket, netadr_t adr, char *format, ...)
{
	va_list		argptr;
	static char		string[MAX_MSGLEN - 4];
	
	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	Netchan_OutOfBand (net_socket, adr, strlen(string), (byte *)string);
}


/*
==============
Netchan_Setup

called to open a channel to a remote system
==============
*/
void Netchan_Setup (netsrc_t sock, netchan_t *chan, netadr_t adr, int qport)
{
	memset (chan, 0, sizeof(*chan));
	
	chan->sock = sock;
	chan->remote_address = adr;
	chan->qport = qport;
	chan->last_received = curtime;
	chan->incoming_sequence = 0;
	chan->outgoing_sequence = 1;

	Buf_Init (&chan->message, chan->message_buf, sizeof(chan->message_buf));
	chan->message.allowoverflow = True;
}


/*
===============
Netchan_CanReliable

Returns True if the last reliable message has acked
================
*/
sboolean Netchan_CanReliable (netchan_t *chan)
{
	if (chan->reliable_length)
		return False;			// waiting for ack
	return True;
}


sboolean Netchan_NeedReliable (netchan_t *chan)
{
	sboolean	send_reliable;

// if the remote side dropped the last reliable message, resend it
	send_reliable = False;

	if (chan->incoming_acknowledged > chan->last_reliable_sequence
	&& chan->incoming_reliable_acknowledged != chan->reliable_sequence)
		send_reliable = True;

// if the reliable transmit buffer is empty, copy the current message out
	if (!chan->reliable_length && chan->message.currentsize)
	{
		send_reliable = True;
	}

	return send_reliable;
}

/*
===============
Netchan_Transmit

tries to send an unreliable message to a connection, and handles the
transmition / retransmition of the reliable messages.

A 0 length will still generate a packet and deal with the reliable messages.
================
*/
void Netchan_Transmit (netchan_t *chan, int length, byte *data)
{
	buffer_t	send;
	byte		send_buf[MAX_MSGLEN];
	sboolean	send_reliable;
	unsigned	w1, w2;

// check for message overflow
	if (chan->message.overflow)
	{
		chan->fatal_error = True;
		printf ("%s:Outgoing message overflow\n"
			, NET_AdrToString (chan->remote_address));
		return;
	}

	send_reliable = Netchan_NeedReliable (chan);

	if (!chan->reliable_length && chan->message.currentsize)
	{
		memcpy (chan->reliable_buf, chan->message_buf, chan->message.currentsize);
		chan->reliable_length = chan->message.currentsize;
		chan->message.currentsize = 0;
		chan->reliable_sequence ^= 1;
	}


// write the packet header
	Buf_Init (&send, send_buf, sizeof(send_buf));

	w1 = ( chan->outgoing_sequence & ~(1<<31) ) | (send_reliable<<31);
	w2 = ( chan->incoming_sequence & ~(1<<31) ) | (chan->incoming_reliable_sequence<<31);

	chan->outgoing_sequence++;
	chan->last_sent = curtime;

	MSG_WriteLong (&send, w1);
	MSG_WriteLong (&send, w2);

	// send the qport if we are a client
	if (chan->sock == NS_CLIENT)
		MSG_WriteShort (&send, sport->value);

// copy the reliable message to the packet first
	if (send_reliable)
	{
		Buf_Write (&send, chan->reliable_buf, chan->reliable_length);
		chan->last_reliable_sequence = chan->outgoing_sequence;
	}
	
// add the unreliable part if space is available
	if (send.maxsize - send.currentsize >= length)
		Buf_Write (&send, data, length);
	else
		printf ("Netchan_Transmit: dumped unreliable\n");

// send the datagram
	NET_SendPacket (chan->sock, send.currentsize, send.data, chan->remote_address);

	if (showpackets->value)
	{
		if (send_reliable)
			printf ("send %4i : s=%i reliable=%i ack=%i rack=%i\n"
				, send.currentsize
				, chan->outgoing_sequence - 1
				, chan->reliable_sequence
				, chan->incoming_sequence
				, chan->incoming_reliable_sequence);
		else
			printf ("send %4i : s=%i ack=%i rack=%i\n"
				, send.currentsize
				, chan->outgoing_sequence - 1
				, chan->incoming_sequence
				, chan->incoming_reliable_sequence);
	}
}

/*
=================
Netchan_Process

called when the current net_message is from remote_address
modifies net_message so that it points to the packet payload
=================
*/
sboolean Netchan_Process (netchan_t *chan, buffer_t *msg)
{
	unsigned	sequence, sequence_ack;
	unsigned	reliable_ack, reliable_message;
	int			qport;

// get sequence numbers		
	MSG_BeginReading (msg);
	sequence = MSG_ReadLong (msg);
	sequence_ack = MSG_ReadLong (msg);

	// read the qport if we are a server
	if (chan->sock == NS_SERVER)
		qport = MSG_ReadShort (msg);

	reliable_message = sequence >> 31;
	reliable_ack = sequence_ack >> 31;

	sequence &= ~(1<<31);
	sequence_ack &= ~(1<<31);	

	if (showpackets->value)
	{
		if (reliable_message)
			printf ("recv %4i : s=%i reliable=%i ack=%i rack=%i\n"
				, msg->currentsize
				, sequence
				, chan->incoming_reliable_sequence ^ 1
				, sequence_ack
				, reliable_ack);
		else
			printf ("recv %4i : s=%i ack=%i rack=%i\n"
				, msg->currentsize
				, sequence
				, sequence_ack
				, reliable_ack);
	}

//
// discard stale or duplicated packets
//
	if (sequence <= chan->incoming_sequence)
	{
		if (showdrop->value)
			printf ("%s:Out of order packet %i at %i\n"
				, NET_AdrToString (chan->remote_address)
				,  sequence
				, chan->incoming_sequence);
		return False;
	}

//
// dropped packets don't keep the message from being used
//
	chan->dropped = sequence - (chan->incoming_sequence+1);
	if (chan->dropped > 0)
	{
		if (showdrop->value)
			printf ("%s:Dropped %i packets at %i\n"
			, NET_AdrToString (chan->remote_address)
			, chan->dropped
			, sequence);
	}

//
// if the current outgoing reliable message has been acknowledged
// clear the buffer to make way for the next
//
	if (reliable_ack == chan->reliable_sequence)
		chan->reliable_length = 0;	// it has been received
	
//
// if this message contains a reliable message, bump incoming_reliable_sequence 
//
	chan->incoming_sequence = sequence;
	chan->incoming_acknowledged = sequence_ack;
	chan->incoming_reliable_acknowledged = reliable_ack;
	if (reliable_message)
	{
		chan->incoming_reliable_sequence ^= 1;
	}

//
// the message can now be read from the current message pointer
//
	chan->last_received = curtime;

	return True;
}

