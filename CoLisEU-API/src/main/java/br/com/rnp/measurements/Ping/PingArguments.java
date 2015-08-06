package br.com.rnp.measurements.Ping;

/**
 * Created by matias on 13/01/15.
 */

public class PingArguments {
    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    String url;
	int count;
	long timeout;
	int payload_bytes;
	long interval;
	int ttl;

	public PingArguments() {

	}

	// TODO make that it can be build

	public static class Builder {

		private PingArguments arguments;

		public Builder() {
			this.arguments = new PingArguments();
		}

		public Builder url(String url) {
			arguments.url = url;
			return this;
		}

		public Builder count(int count) {
			arguments.count = count;
			return this;
		}

		public Builder timeout(long timeout) {
			arguments.timeout = timeout;
			return this;
		}

		public Builder bytes(int bytes) {
			arguments.payload_bytes = bytes;
			return this;
		}

		public Builder ttl(int ttl) {
			arguments.ttl = ttl;
			return this;
		}

		/**
		 * -i
		 * 
		 * @param interval
		 * @return
		 */
		public Builder interval(long interval) {
			arguments.interval = interval;
			return this;
		}

		public PingArguments build() {
			return arguments;
		}
	}

	// TODO make adaptable if certain properaties aren't set
	public String getCommand() {
		StringBuilder b = new StringBuilder();

		b.append("ping").append(" ").append("-c").append(" ").append(count)
				.append(" ")

				.append("-W").append(" ").append(timeout).append(" ")

				.append("-s").append(" ").append(payload_bytes).append(" ")
				.append(url.replaceFirst("https://", "").split("/")[0]);

		return b.toString();
	}
}