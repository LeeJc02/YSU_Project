new Vue({
    el: '#app',
    data: {
        transactions: [],   // 存储交易记录
        currentPage: 1,     // 当前页码
        totalPages: 1,      // 总页数
        transactionsPerPage: 10, // 每页显示的交易记录数
        transactionTypes: {  // 枚举值和中文的映射
            deposit: '存款',
            withdraw: '取款',
            transfer: '转账',
        },
        searchCardNumber: ''  // 存储用户输入的卡号
    },
    created() {
        this.getTransactionHistory();  // 页面加载时获取交易记录
    },
    methods: {
        // 返回上一页
        goBack() {
            window.history.back();  // 返回上一页
        },

        // 获取所有交易历史
        getTransactionHistory() {
            axios.post('/admin/transactions', {
                page: this.currentPage,  // 当前页码
                size: this.transactionsPerPage  // 每页的交易记录数
            })
                .then(response => {
                    const allTransactions = response.data.data;  // 后端返回的交易记录数组
                    this.totalPages = response.data.totalPages;  // 后端返回的总页数

                    // 按 timestamp 字段进行降序排序，时间越近的交易越靠前
                    const sortedTransactions = allTransactions.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));

                    // 为交易记录重新分配交易ID（如果需要）
                    this.transactions = sortedTransactions.map((transaction, index) => {
                        transaction.transactionId = (this.currentPage - 1) * this.transactionsPerPage + index + 1;
                        return transaction;
                    });
                })
                .catch(error => {
                    alert('获取交易流水失败，请稍后再试');
                });
        },

        // 根据卡号查询交易记录
        getTransactionsByCardNumber() {
            if (!this.searchCardNumber) {
                this.getTransactionHistory();  // 如果未输入卡号，加载所有交易流水
                return;
            }

            axios.post('/admin/transaction', { cardNumber: this.searchCardNumber })  // 根据卡号获取交易记录
                .then(response => {
                    const allTransactions = response.data.data;
                    if (allTransactions && allTransactions.length > 0) {
                        this.transactions = allTransactions;
                        this.totalPages = 1;  // 假设卡号查询只有一页数据
                        this.currentPage = 1;
                    } else {
                        this.transactions = [];
                        alert('未找到该卡号的交易记录');
                    }
                })
                .catch(error => {
                    alert('查询失败，请稍后再试');
                });
        },

        // 获取中文的交易类型
        getTransactionType(type) {
            return this.transactionTypes[type] || type;  // 如果没有匹配的类型，返回原类型
        },

        // 上一页
        previousPage() {
            if (this.currentPage > 1) {
                this.currentPage--;
                this.getTransactionHistory();  // 获取上一页的交易记录
            }
        },

        // 下一页
        nextPage() {
            if (this.currentPage < this.totalPages) {
                this.currentPage++;
                this.getTransactionHistory();  // 获取下一页的交易记录
            }
        }
    }
});
