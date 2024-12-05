#ifndef __BACKPORT_NET_NETLINK_H
#define __BACKPORT_NET_NETLINK_H
#include_next <net/netlink.h>
#include <linux/version.h>
#include <linux/in6.h>

#if LINUX_VERSION_IS_LESS(5,2,0)
/* can't backport using the enum - need to override */
#define NLA_UNSPEC		0
#define NLA_U8			1
#define NLA_U16			2
#define NLA_U32			3
#define NLA_U64			4
#define NLA_STRING		5
#define NLA_FLAG		6
#define NLA_MSECS		7
#define NLA_NESTED		8
#define NLA_NESTED_ARRAY	9
#define NLA_NUL_STRING		10
#define NLA_BINARY		11
#define NLA_S8			12
#define NLA_S16			13
#define NLA_S32			14
#define NLA_S64			15
#define NLA_BITFIELD32		16
#define NLA_REJECT		17
#define NLA_EXACT_LEN		18
#define NLA_EXACT_LEN_WARN	19
#define NLA_MIN_LEN		20
#define __NLA_TYPE_MAX		21
#define NLA_TYPE_MAX		(__NLA_TYPE_MAX - 1)

struct backport_nla_policy {
	u8		type;
	u8		validation_type;
	u16		len;
	union {
		const void *validation_data;
		struct {
			s16 min, max;
		};
		int (*validate)(const struct nlattr *attr,
				struct netlink_ext_ack *extack);
		u16 strict_start_type;
	};
};
#define nla_policy backport_nla_policy

#define nla_nest_start_noflag LINUX_BACKPORT(nla_nest_start_noflag)
static inline struct nlattr *nla_nest_start_noflag(struct sk_buff *skb,
						   int attrtype)
{
	struct nlattr *start = (struct nlattr *)skb_tail_pointer(skb);

	if (nla_put(skb, attrtype, 0, NULL) < 0)
		return NULL;

	return start;
}

#define nla_nest_start LINUX_BACKPORT(nla_nest_start)
static inline struct nlattr *nla_nest_start(struct sk_buff *skb, int attrtype)
{
	return nla_nest_start_noflag(skb, attrtype | NLA_F_NESTED);
}

enum netlink_validation {
	NL_VALIDATE_LIBERAL = 0,
	NL_VALIDATE_TRAILING = BIT(0),
	NL_VALIDATE_MAXTYPE = BIT(1),
	NL_VALIDATE_UNSPEC = BIT(2),
	NL_VALIDATE_STRICT_ATTRS = BIT(3),
	NL_VALIDATE_NESTED = BIT(4),
};

#define NL_VALIDATE_DEPRECATED_STRICT (NL_VALIDATE_TRAILING |\
				       NL_VALIDATE_MAXTYPE)
#define NL_VALIDATE_STRICT (NL_VALIDATE_TRAILING |\
			    NL_VALIDATE_MAXTYPE |\
			    NL_VALIDATE_UNSPEC |\
			    NL_VALIDATE_STRICT_ATTRS |\
			    NL_VALIDATE_NESTED)

#define __nla_validate LINUX_BACKPORT(__nla_validate)
int __nla_validate(const struct nlattr *head, int len, int maxtype,
		   const struct nla_policy *policy, unsigned int validate,
		   struct netlink_ext_ack *extack);
#define __nla_parse LINUX_BACKPORT(__nla_parse)
int __nla_parse(struct nlattr **tb, int maxtype, const struct nlattr *head,
		int len, const struct nla_policy *policy, unsigned int validate,
		struct netlink_ext_ack *extack);

#define nla_policy_len LINUX_BACKPORT(nla_policy_len)
int nla_policy_len(const struct nla_policy *, int);

#define nla_parse LINUX_BACKPORT(nla_parse)
static inline int nla_parse(struct nlattr **tb, int maxtype,
			    const struct nlattr *head, int len,
			    const struct nla_policy *policy,
			    struct netlink_ext_ack *extack)
{
	return __nla_parse(tb, maxtype, head, len, policy,
			   NL_VALIDATE_STRICT, extack);
}

#define nla_parse_deprecated LINUX_BACKPORT(nla_parse_deprecated)
static inline int nla_parse_deprecated(struct nlattr **tb, int maxtype,
				       const struct nlattr *head, int len,
				       const struct nla_policy *policy,
				       struct netlink_ext_ack *extack)
{
	return __nla_parse(tb, maxtype, head, len, policy,
			   NL_VALIDATE_LIBERAL, extack);
}

#define nla_parse_deprecated_strict LINUX_BACKPORT(nla_parse_deprecated_strict)
static inline int nla_parse_deprecated_strict(struct nlattr **tb, int maxtype,
					      const struct nlattr *head,
					      int len,
					      const struct nla_policy *policy,
					      struct netlink_ext_ack *extack)
{
	return __nla_parse(tb, maxtype, head, len, policy,
			   NL_VALIDATE_DEPRECATED_STRICT, extack);
}

#define __nlmsg_parse LINUX_BACKPORT(__nlmsg_parse)
static inline int __nlmsg_parse(const struct nlmsghdr *nlh, int hdrlen,
				struct nlattr *tb[], int maxtype,
				const struct nla_policy *policy,
				unsigned int validate,
				struct netlink_ext_ack *extack)
{
	if (nlh->nlmsg_len < nlmsg_msg_size(hdrlen)) {
		NL_SET_ERR_MSG(extack, "Invalid header length");
		return -EINVAL;
	}

	return __nla_parse(tb, maxtype, nlmsg_attrdata(nlh, hdrlen),
			   nlmsg_attrlen(nlh, hdrlen), policy, validate,
			   extack);
}

#define nlmsg_parse LINUX_BACKPORT(nlmsg_parse)
static inline int nlmsg_parse(const struct nlmsghdr *nlh, int hdrlen,
			      struct nlattr *tb[], int maxtype,
			      const struct nla_policy *policy,
			      struct netlink_ext_ack *extack)
{
	return __nla_parse(tb, maxtype, nlmsg_attrdata(nlh, hdrlen),
			   nlmsg_attrlen(nlh, hdrlen), policy,
			   NL_VALIDATE_STRICT, extack);
}

#define nlmsg_parse_deprecated LINUX_BACKPORT(nlmsg_parse_deprecated)
static inline int nlmsg_parse_deprecated(const struct nlmsghdr *nlh, int hdrlen,
					 struct nlattr *tb[], int maxtype,
					 const struct nla_policy *policy,
					 struct netlink_ext_ack *extack)
{
	return __nlmsg_parse(nlh, hdrlen, tb, maxtype, policy,
			     NL_VALIDATE_LIBERAL, extack);
}

#define nlmsg_parse_deprecated_strict LINUX_BACKPORT(nlmsg_parse_deprecated_strict)
static inline int
nlmsg_parse_deprecated_strict(const struct nlmsghdr *nlh, int hdrlen,
			      struct nlattr *tb[], int maxtype,
			      const struct nla_policy *policy,
			      struct netlink_ext_ack *extack)
{
	return __nlmsg_parse(nlh, hdrlen, tb, maxtype, policy,
			     NL_VALIDATE_DEPRECATED_STRICT, extack);
}

#define nla_validate_deprecated LINUX_BACKPORT(nla_validate_deprecated)
static inline int nla_validate_deprecated(const struct nlattr *head, int len,
					  int maxtype,
					  const struct nla_policy *policy,
					  struct netlink_ext_ack *extack)
{
	return __nla_validate(head, len, maxtype, policy, NL_VALIDATE_LIBERAL,
			      extack);
}

#define nla_validate LINUX_BACKPORT(nla_validate)
static inline int nla_validate(const struct nlattr *head, int len, int maxtype,
			       const struct nla_policy *policy,
			       struct netlink_ext_ack *extack)
{
	return __nla_validate(head, len, maxtype, policy, NL_VALIDATE_STRICT,
			      extack);
}

#define nlmsg_validate_deprecated LINUX_BACKPORT(nlmsg_validate_deprecated)
static inline int nlmsg_validate_deprecated(const struct nlmsghdr *nlh,
					    int hdrlen, int maxtype,
					    const struct nla_policy *policy,
					    struct netlink_ext_ack *extack)
{
	if (nlh->nlmsg_len < nlmsg_msg_size(hdrlen))
		return -EINVAL;

	return __nla_validate(nlmsg_attrdata(nlh, hdrlen),
			      nlmsg_attrlen(nlh, hdrlen), maxtype,
			      policy, NL_VALIDATE_LIBERAL, extack);
}

#define nlmsg_validate LINUX_BACKPORT(nlmsg_validate)
static inline int nlmsg_validate(const struct nlmsghdr *nlh,
				 int hdrlen, int maxtype,
				 const struct nla_policy *policy,
				 struct netlink_ext_ack *extack)
{
	if (nlh->nlmsg_len < nlmsg_msg_size(hdrlen))
		return -EINVAL;

	return __nla_validate(nlmsg_attrdata(nlh, hdrlen),
			      nlmsg_attrlen(nlh, hdrlen), maxtype,
			      policy, NL_VALIDATE_STRICT, extack);
}

#define nla_parse_nested LINUX_BACKPORT(nla_parse_nested)
static inline int nla_parse_nested(struct nlattr *tb[], int maxtype,
				   const struct nlattr *nla,
				   const struct nla_policy *policy,
				   struct netlink_ext_ack *extack)
{
	if (!(nla->nla_type & NLA_F_NESTED)) {
		NL_SET_ERR_MSG_ATTR(extack, nla, "NLA_F_NESTED is missing");
		return -EINVAL;
	}

	return __nla_parse(tb, maxtype, nla_data(nla), nla_len(nla), policy,
			   NL_VALIDATE_STRICT, extack);
}

#define nla_parse_nested_deprecated LINUX_BACKPORT(nla_parse_nested_deprecated)
static inline int nla_parse_nested_deprecated(struct nlattr *tb[], int maxtype,
					      const struct nlattr *nla,
					      const struct nla_policy *policy,
					      struct netlink_ext_ack *extack)
{
	return __nla_parse(tb, maxtype, nla_data(nla), nla_len(nla), policy,
			   NL_VALIDATE_LIBERAL, extack);
}

#define __nla_validate_nested LINUX_BACKPORT(__nla_validate_nested)
static inline int __nla_validate_nested(const struct nlattr *start, int maxtype,
					const struct nla_policy *policy,
					unsigned int validate,
					struct netlink_ext_ack *extack)
{
	return __nla_validate(nla_data(start), nla_len(start), maxtype, policy,
			      validate, extack);
}

#define nla_validate_nested_deprecated LINUX_BACKPORT(nla_validate_nested_deprecated)
static inline int
nla_validate_nested_deprecated(const struct nlattr *start, int maxtype,
			       const struct nla_policy *policy,
			       struct netlink_ext_ack *extack)
{
	return __nla_validate_nested(start, maxtype, policy,
				     NL_VALIDATE_LIBERAL, extack);
}

#define NLA_POLICY_MIN_LEN(_len)	{ .type = NLA_MIN_LEN, .len = _len }
#endif /* < 5.2 */

#if LINUX_VERSION_IS_LESS(5,6,0)
#define nla_validate_nested LINUX_BACKPORT(nla_validate_nested)
static inline int
nla_validate_nested(const struct nlattr *start, int maxtype,
		    const struct nla_policy *policy,
		    struct netlink_ext_ack *extack)
{
	return __nla_validate_nested(start, maxtype, policy,
				     NL_VALIDATE_STRICT, extack);
}
#endif /* < 5.6 */

#if LINUX_VERSION_IS_LESS(5,1,0)
#undef NLA_POLICY_NESTED
#undef NLA_POLICY_NESTED_ARRAY
#define _NLA_POLICY_NESTED(maxattr, policy) \
	{ .type = NLA_NESTED, .validation_data = policy, .len = maxattr }
#define _NLA_POLICY_NESTED_ARRAY(maxattr, policy) \
	{ .type = NLA_NESTED_ARRAY, .validation_data = policy, .len = maxattr }
#define NLA_POLICY_NESTED(policy) \
	_NLA_POLICY_NESTED(ARRAY_SIZE(policy) - 1, policy)
#define NLA_POLICY_NESTED_ARRAY(policy) \
	_NLA_POLICY_NESTED_ARRAY(ARRAY_SIZE(policy) - 1, policy)
#endif /* < 5.1 */

#if LINUX_VERSION_IS_LESS(4,20,0)
enum nla_policy_validation {
	NLA_VALIDATE_NONE,
	NLA_VALIDATE_RANGE,
	NLA_VALIDATE_MIN,
	NLA_VALIDATE_MAX,
	NLA_VALIDATE_FUNCTION,
};

#define NLA_POLICY_EXACT_LEN(_len)	{ .type = NLA_EXACT_LEN, .len = _len }
#define NLA_POLICY_EXACT_LEN_WARN(_len)	{ .type = NLA_EXACT_LEN_WARN, \
					  .len = _len }

#define NLA_POLICY_ETH_ADDR		NLA_POLICY_EXACT_LEN(ETH_ALEN)
#define NLA_POLICY_ETH_ADDR_COMPAT	NLA_POLICY_EXACT_LEN_WARN(ETH_ALEN)

#define __NLA_ENSURE(condition) (sizeof(char[1 - 2*!(condition)]) - 1)
#define NLA_ENSURE_INT_TYPE(tp)				\
	(__NLA_ENSURE(tp == NLA_S8 || tp == NLA_U8 ||	\
		      tp == NLA_S16 || tp == NLA_U16 ||	\
		      tp == NLA_S32 || tp == NLA_U32 ||	\
		      tp == NLA_S64 || tp == NLA_U64) + tp)
#define NLA_ENSURE_NO_VALIDATION_PTR(tp)		\
	(__NLA_ENSURE(tp != NLA_BITFIELD32 &&		\
		      tp != NLA_REJECT &&		\
		      tp != NLA_NESTED &&		\
		      tp != NLA_NESTED_ARRAY) + tp)

#define NLA_POLICY_RANGE(tp, _min, _max) {		\
	.type = NLA_ENSURE_INT_TYPE(tp),		\
	.validation_type = NLA_VALIDATE_RANGE,		\
	.min = _min,					\
	.max = _max					\
}

#define NLA_POLICY_MIN(tp, _min) {			\
	.type = NLA_ENSURE_INT_TYPE(tp),		\
	.validation_type = NLA_VALIDATE_MIN,		\
	.min = _min,					\
}

#define NLA_POLICY_MAX(tp, _max) {			\
	.type = NLA_ENSURE_INT_TYPE(tp),		\
	.validation_type = NLA_VALIDATE_MAX,		\
	.max = _max,					\
}

#define NLA_POLICY_VALIDATE_FN(tp, fn, ...) {		\
	.type = NLA_ENSURE_NO_VALIDATION_PTR(tp),	\
	.validation_type = NLA_VALIDATE_FUNCTION,	\
	.validate = fn,					\
	.len = __VA_ARGS__ + 0,				\
}
#endif /* < 4.20 */

#if LINUX_VERSION_IS_LESS(5,10,0)
// pre-declare all the minimum lengths in use
#define MIN_LEN_VALIDATION(n)						\
static inline								\
int nla_validate_min_len_##n(const struct nlattr *attr,			\
			     struct netlink_ext_ack *extack)		\
{									\
	if (nla_len(attr) < n)						\
		return -EINVAL;						\
	return 0;							\
}

MIN_LEN_VALIDATION(2)
MIN_LEN_VALIDATION(13)
MIN_LEN_VALIDATION(16)
MIN_LEN_VALIDATION(42)

// double-expansion to expand _min to the actual value
#define NLA_POLICY_BINARY_RANGE(_min, _max) _NLA_POLICY_BINARY_RANGE(_min, _max)
#define _NLA_POLICY_BINARY_RANGE(_min, _max)		\
{							\
	.type = NLA_BINARY,				\
	.len = _max,					\
	.validation_type = NLA_VALIDATE_FUNCTION,	\
	.validate = nla_validate_min_len_ ## _min,	\
}
#else
#define NLA_POLICY_BINARY_RANGE(_min, _max) NLA_POLICY_RANGE(NLA_BINARY, _min, _max)
#endif /* < 5.10 */

#endif /* __BACKPORT_NET_NETLINK_H */
