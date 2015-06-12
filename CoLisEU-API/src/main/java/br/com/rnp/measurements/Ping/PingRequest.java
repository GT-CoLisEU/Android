package br.com.rnp.measurements.Ping;

/**
 * Created by matias on 13/01/15.
 */

public class PingRequest {

	private final long bytes;
	private final String from;

	@Override
	public String toString() {
		return "PingRequest [bytes=" + bytes + ", from=" + from + ", fromIP="
				+ fromIP + ", reqnr=" + reqnr + ", ttl=" + ttl + ", time="
				+ time + "]";
	}

	private final String fromIP;
	private final int reqnr;
	private final int ttl;
	private final float time;

	public long bytes() {
		return bytes;
	}

	public String from() {
		return from;
	}

	public String fromIP() {
		return fromIP;
	}

	public int reqNr() {
		return reqnr;
	}

	public int ttl() {
		return ttl;
	}

	public float time() {
		return time;
	}

	protected PingRequest(long bytes, String from, String fromIP, int reqnr,
			int ttl, float time) {
		super();
		this.bytes = bytes;
		this.from = from;
		this.fromIP = fromIP;
		this.reqnr = reqnr;
		this.ttl = ttl;
		this.time = time;
	}

	public static PingRequestBuilder builder() {
		return new PingRequestBuilder();
	}

	protected static class PingRequestBuilder {
		private long bytes;
		private String from;
		private String fromIP;
		private int reqnr;
		private int ttl;
		private float time;

		public PingRequestBuilder() {

		}

		public PingRequestBuilder bytes(long bytes) {
			this.bytes = bytes;
			return this;
		}

		public PingRequestBuilder from(String from) {
			this.from = from;
			return this;
		}

		public PingRequestBuilder fromIP(String fromIP) {
			this.fromIP = fromIP;
			return this;
		}

		public PingRequestBuilder reqNr(int reqnr) {
			this.reqnr = reqnr;
			return this;
		}

		public PingRequestBuilder ttl(int ttl) {
			this.ttl = ttl;
			return this;
		}

		public PingRequestBuilder time(float time) {
			this.time = time;
			return this;
		}

		public PingRequest build() {
			return new PingRequest(bytes, from, fromIP, reqnr, ttl, time);
		}

	}

}
