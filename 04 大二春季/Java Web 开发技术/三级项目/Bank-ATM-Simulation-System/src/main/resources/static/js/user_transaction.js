new Vue({
    el: '#app',
    data: {
        transactions: [],  // 用于存储交易记录
        transactionTypes: {  // 枚举值和中文的映射
            deposit: '存款',
            withdraw: '取款',
            transfer: '转账',
        }
    },
    created() {
        this.getTransactionHistory();  // 页面加载时调用获取交易历史的方法
    },
    methods: {
        // 返回上一页
        goBack() {
            window.history.back();  // 直接返回上一页
        },

        // 获取交易历史
        getTransactionHistory() {
            // 假设后端接口已经根据 session 中的 currentUser 获取卡号
            axios.post('/account/transaction')  // 发送 POST 请求
                .then(response => {
                    const allTransactions = response.data.data;  // 假设后端返回的数据中有 data 字段

                    // 按 timestamp 字段进行降序排序，时间越近的交易越靠前
                    const sortedTransactions = allTransactions.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));

                    // 重新为交易记录分配交易ID，时间越近的交易ID越大
                    this.transactions = sortedTransactions.map((transaction, index) => {
                        transaction.transactionId = index + 1;  // 重新分配ID，按时间顺序从1开始
                        return transaction;
                    });
                })
                .catch(error => {
                    alert('获取交易流水失败，请稍后再试');
                });
        },

        // 获取交易类型的中文名称
        getTransactionType(type) {
            return this.transactionTypes[type] || '未知';
        }
    }
});